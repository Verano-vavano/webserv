#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_error_code(t_response_creator &r) {
	//std::cout << "ERROR CODE = " << r.err_code << std::endl;

	std::vector<HTTPConfig::t_error>	&errors = r.conf->error_page;
	std::vector<HTTPConfig::t_error>::const_iterator it;
	bool	found = false;

	for (it = errors.begin(); it != errors.end(); it++) {
		for (std::set<int>::const_iterator it2 = it->codes.begin();
				it2 != it->codes.end(); it2++) {
			if (*it2 == r.err_code) { found = true; break ; }
		}
		if (found) { break ; }
	}

	if (found) {
		get_body(it->uri, r, it->response);
	} else if (r.err_code != 200) {
		r.res.body = "[DEFAULT ERROR] " + get_error_tag(r.err_code);
	}
	return ;

}

std::string HTTPProtocol::get_error_tag(int err_code) {
	std::ostringstream	s;
	s << err_code;
	std::string	err_mes;
	switch (err_code) {
		case 200:
			err_mes = "OK";
			break ;
		case 403:
			err_mes = "Forbidden";
			break ;
		case 404:
			err_mes = "Not Found";
			break ;
		case 406:
			err_mes = "Not Acceptable";
			break ;
		case 500:
			err_mes = "Internal Servor Error";
			break ;
		case 501:
			err_mes = "Not Implemented";
			break ;
		case 505:
			err_mes = "HTTP Version Not Supported";
			break ;
		default:
			err_mes = "Unknown";
	}
	return (s.str() + " " + err_mes);
}
