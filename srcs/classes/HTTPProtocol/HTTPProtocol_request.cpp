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
	r.req.method = line.substr(0, sub_index);
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
	s = s.substr(index);
}

// Checks for CRLFCRLF
bool	HTTPProtocol::check_div_end(std::string const & buf) {
	static short	state;

	for (std::string::const_iterator it = buf.begin(); it != buf.end(); it++) {
		if (*it != '\r' && *it != '\n') { if (state) { state = 0; } }
		else if (state % 2 == 0 && *it == '\r') { state++; }
		else if (state % 2 == 1 && *it == '\n') {
			state++;
			if (state == 4) { state = 0; return (true); }
		}
		else { state = 0; }
	}
	return (false);
}

// Empties the file descriptor so that it's fully read
void	HTTPProtocol::empty_fd_in(int fd) {
	int	bytes;

	if (ioctl(fd, FIONREAD, &bytes) == -1 || bytes <= 0) { return ; }
	recv(fd, NULL, bytes, 0);
}

short	HTTPProtocol::read_crlfcrlf(int fd, t_response_creator &r, long buf_size, std::string & req, unsigned long length) {
	char		buffer[buf_size];
	std::string	buf_to_str;
	long		ret;
	long		to_read = buf_size - 1;
	bool		sized = (length != 0);

	if (static_cast<long> (length) > r.conf->client_max_body_size) { r.err_code = 413; return (1); }
	// We loop indefinitely until we reach the first CRLFCRLF, marking the end of the headers
	do {
		if (length) { to_read = std::min(length, static_cast<unsigned long> (buf_size - 1)); }
		ret = recv(fd, buffer, to_read, 0);
		if (ret == -1) { r.err_code = 500; this->empty_fd_in(fd); return (1); } // RECV FAILED. Internal Server Error
		else if (ret == 0 && length == 0) { return (0); } // Nothing has been read. The client disconnected
		else if (ret == 0) { r.err_code = 400; this->empty_fd_in(fd); return (1); } // NO CRLFCRLF. Bad request
		buffer[ret] = '\0';
		buf_to_str = std::string(buffer);
		req += buf_to_str;
		length -= ret;
	} while (ret == buf_size - 1 && !this->check_div_end(buf_to_str) && (!sized || length));

	return (-1);
}

// Method to read client EPOLLIN and parse it into r.req variable.
// -1 = Fatal error (no use case for now)
// 0 = Client disconnection
// 1 = No Error
int	HTTPProtocol::read_and_understand_request(int fd, t_response_creator &r) {
	std::string	request;
	short	ret;

	// HEADERS reading
	ret = this->read_crlfcrlf(fd, r, r.conf->client_header_buffer_size, request, 0);
	if (ret != -1) { return (ret); }

	this->parse_headers(request, r);
	request = request.substr(3);
	// If GET, no body required. Optimised to skip
	if (r.req.method == "GET") { this->empty_fd_in(fd); return (1); }

	std::map<std::string, std::vector<std::string> >::const_iterator	finder = r.req.headers.find("content-length");
	if (finder == r.req.headers.end()) { r.err_code = 411; this->empty_fd_in(fd); return (1); } // No Content-Length header (required for body length)

	long	length = atol(finder->second[0].c_str()) -  request.size();

	// BODY reading
	if (length != 0)
		ret = this->read_crlfcrlf(fd, r, r.conf->client_body_buffer_size, request, length);

	if (ret == -1 || length == 0)
		r.req.body = request;
	this->empty_fd_in(fd);

	return (ret == -1 ? 1: ret);
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

void	HTTPProtocol::empty_request(t_request &req) {
	req.method = "";
	req.uri = "";
	req.http_version = "";
	req.headers.clear();
	req.body = "";
}
