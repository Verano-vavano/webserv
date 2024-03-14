#include "HTTPProtocol.hpp"

void	HTTPProtocol::create_response(t_response_creator &rc) {
	rc.err_code = 200;
	rc.res.status_line = "";
	rc.res.headers = "";
	rc.res.body = "";

	this->handle_method(rc); // Gets body from request method
	rc.file_type = get_mime_type(rc.conf, rc.file_type);
	if (rc.err_code == 200)
		this->check_type(rc); // Checks if file type matches Accept header
	this->handle_error_code(rc); // Gets body if error
	this->set_headers(rc); // Sets headers wow
	rc.res.status_line = "HTTP/1.1 " + this->get_error_tag(rc.err_code);
	std::cout << std::endl << "RESPONSE = " << std::endl;
	std::cout << rc.res.status_line + CRLF + rc.res.headers + CRLF + rc.res.body << "END RESPONSE" << std::endl;
}

void	HTTPProtocol::handle_method(t_response_creator &r) {
	if (r.req.method == "GET") {
		this->handle_get(r);
	}
	else
		r.err_code = 501;
}

std::string HTTPProtocol::get_error_tag(int err_code) {
	std::ostringstream	s;
	s << err_code;
	std::string	err_mes;
	switch (err_code) {
		case 200:
			err_mes = "OK";
			break ;
		case 404:
			err_mes = "Not Found";
			break ;
		case 406:
			err_mes = "Not Acceptable";
			break ;
		case 501:
			err_mes = "Not Implemented";
			break ;
		default:
			err_mes = "Unknown";
	}
	return (s.str() + " " + err_mes);
}

std::string	HTTPProtocol::format_response(t_response &res) {
	return (res.status_line + CRLF + res.headers + CRLF + res.body);
}
