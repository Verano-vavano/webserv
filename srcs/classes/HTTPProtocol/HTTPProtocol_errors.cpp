#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_error_code(t_response_creator &r) {
	std::cout << "ERROR CODE = " << r.err_code << std::endl;

	std::vector<HTTPConfig::t_error>	&errors = r.conf->error_page;
	std::vector<HTTPConfig::t_error>::const_iterator it;
	bool	found = false;

	for (it = errors.begin(); it != errors.end(); it++) {
		for (std::vector<int>::const_iterator it2 = it->codes.begin();
				it2 != it->codes.end(); it2++) {
			if (*it2 == r.err_code) { found = true; break ; }
		}
		if (found) { break ; }
	}

	if (found) {
		std::string		full_uri = this->get_complete_uri(it->uri, r.conf);
		std::cout << "URI = [" << r.conf->path + full_uri << "]" << std::endl;
		std::ifstream	file((r.conf->path + full_uri).c_str());
		if (file && file.good()) {
			std::cout << "INSIDE" << std::endl;
			if (it->response != -1)
				r.err_code = it->response;
			this->read_entire_file(r.res.body, file);
			return ;
		}
	}

	if (r.err_code != 200) {
		r.res.body = "<!DOCTYPE html><body>[DEFAULT ERROR] " + get_error_tag(r.err_code) + "</body>";
	}
	return ;

}
