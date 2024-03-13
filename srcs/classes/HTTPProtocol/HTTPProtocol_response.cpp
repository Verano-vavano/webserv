#include "HTTPProtocol.hpp"

void	HTTPProtocol::create_response(t_response_creator &rc) {
	std::cout << "Coucou!!" << std::endl;
	rc.err_code = 200;
	rc.res.status_line = "";
	rc.res.headers = "";
	rc.res.body = "";

	this->handle_method(rc);
	this->handle_error_code(rc);
	this->set_headers(rc);
	this->set_status_line(rc);
}

void	HTTPProtocol::handle_method(t_response_creator &r) {
	if (r.req.method == "GET") {
		this->handle_get(r);
	}
	else
		r.err_code = 500;
}

void	HTTPProtocol::set_status_line(t_response_creator &r) {
	std::ostringstream	s;
	s << r.err_code;
	r.res.status_line = "HTTP/1.1 " + s.str();
	std::string	err_mes;
	switch (r.err_code) {
		case 404:
			err_mes = "Not Found";
			break ;
		case 406:
			err_mes = "Not Acceptable";
			break ;
		default:
			err_mes = "OK";
	}
	r.res.status_line += " " + err_mes;
}

std::string	HTTPProtocol::format_response(t_response &res) {
	return (res.status_line + CRLF + res.headers + CRLF + res.body);
}
