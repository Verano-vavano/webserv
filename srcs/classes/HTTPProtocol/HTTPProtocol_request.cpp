#include "HTTPProtocol.hpp"
#include <sys/socket.h>
#include <sys/ioctl.h>

void	HTTPProtocol::parse_headers(std::string & s, t_response_creator & r) {
	std::string line;
	int		 index = 0;
	int		 sub_index = 0;
	int		 sub_sub_index = 0;

	// REQUEST LINE
	index = s.find("\r\n");
	line = s.substr(0, index);

	sub_index = line.find(' '); // after method
	r.req.method = HTTPConfig::to_upper(line.substr(0, sub_index));
	sub_index++;
	sub_sub_index = line.find(' ', sub_index); // after either HTTP or uri
	if (sub_sub_index < index) {
		r.req.uri = line.substr(sub_index, sub_sub_index - sub_index);
		sub_index = sub_sub_index + 1;
	}
	else
		r.req.uri = "NONE";
	r.req.http_version = line.substr(sub_index, index - sub_index);

	// HEADERS are defined by a name and a list of values
	sub_index = index + 2; // sub_index points to the start of the headers and will move line to line
	index = s.find("\r\n\r\n", sub_index) + 1; // index points to the end of the headers
						   // EDIT : it shouldnt, but it still works for whatever reason, so lets keep it at that

	std::pair<std::string, std::vector<std::string> > new_el;
	while (sub_index < index) {
		sub_sub_index = s.find(':', sub_index);
		new_el.first = s.substr(sub_index, sub_sub_index - sub_index);
		// If \n in key, then bad s lol
		if (new_el.first.find("\n") != std::string::npos) {
			r.err_code = 400;
			return ;
		}
		//HTTP key are case insensitive, so store them as lowercase
		for (size_t i = 0 ; i < new_el.first.size() ; i++) {
			new_el.first[i] = tolower(new_el.first[i]);
		}
		sub_sub_index++;
		sub_index = s.find("\r\n", sub_index);
		new_el.second = this->split_header_val(s.substr(sub_sub_index, sub_index - sub_sub_index));
		r.req.headers.insert(new_el);
		sub_index += 2;
	}
	s = s.substr(index + 3);
}

std::vector<std::string>	HTTPProtocol::split_header_val(std::string val) {
	std::vector<std::string>	ret;
	std::string				 sub;
	unsigned int start, end, next;

	start = val.find_first_not_of(' ');
	while (start < val.size()) {
		next = val.find(',', start);
		end = val.find_first_not_of(' ', next);
		ret.push_back(val.substr(start, end - start));
		if (next >= val.size())
			break ;
		start = val.find_first_not_of(' ', next + 1);
	}
	return (ret);
}

// Empties the file descriptor so that it's fully read
void	HTTPProtocol::empty_fd_in(int fd) {
	int	bytes;

	if (ioctl(fd, FIONREAD, &bytes) == -1 || bytes <= 0) { return ; }
	recv(fd, NULL, bytes, 0);
}

void	HTTPProtocol::get_right_conf(t_response_creator &r, std::vector<HTTPConfig::t_config *> &pc) {
	std::map<std::string, std::vector<std::string> >::const_iterator	finder = r.req.headers.find("host");
	if (finder == r.req.headers.end()) { return ; }

	std::string const host_header = (finder->second)[0];
	size_t	idx = host_header.find(':');
	std::string	hostname;
	if (idx == std::string::npos) {
		hostname = (finder->second)[0];
	} else {
		hostname = host_header.substr(0, idx);
	}
	for (std::vector<HTTPConfig::t_config *>::iterator it = pc.begin(); it != pc.end(); it++) {
		if ((*it)->server_name == hostname) {
			r.conf = *it;
			break ;
		}
	}
}


// Method to read client EPOLLIN and parse it into r.req variable.
// -1 = Fatal error (no use case for now)
// 0 = Client disconnection
// 1 = No Error (at least, no error that breaks it all)
int	HTTPProtocol::read_and_understand_request(int fd, t_response_creator &r, std::vector<HTTPConfig::t_config *> pc) {
	long	to_read;
	if (r.conf)
		to_read = r.req.headers_defined ? r.conf->client_body_buffer_size: r.conf->client_header_buffer_size;
	else
		to_read = r.req.headers_defined ? pc[0]->client_body_buffer_size: pc[0]->client_header_buffer_size;

	char	buffer[to_read + 1];
	int		ret = recv(fd, buffer, to_read, 0);

	if (ret == -1) { this->empty_fd_in(fd); r.err_code = 500; return (0); }
	else if (ret == 0) { return (0); }

	buffer[ret] = '\0';
	r.temp_req += buffer;

	if (!r.req.headers_defined && r.temp_req.find("\r\n\r\n") != std::string::npos) {
		r.req.headers_defined = true;
		this->parse_headers(r.temp_req, r);
		if (!r.conf) { this->get_right_conf(r, pc); }
		if (!r.conf) { r.conf = pc[0]; }
	}
	if (r.req.headers_defined && (r.temp_req.size() > LONG_MAX || static_cast<long>(r.temp_req.size()) > r.conf->client_max_body_size)) {
		r.err_code = 413;
		return (1);
	}

	return (1);
}


void	HTTPProtocol::empty_request(t_request &req) {
	req.method = "";
	req.uri = "";
	req.http_version = "";
	req.headers.clear();
	req.body = "";
	req.headers_defined = false;
}
