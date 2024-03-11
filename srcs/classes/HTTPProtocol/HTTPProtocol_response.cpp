#include "HTTPProtocol.hpp"

void	HTTPProtocol::create_response(t_response_creator &rc) {
	rc.err_code = 200;
	rc.res.status_line = "HTTP/1.1";

	this->handle_method(rc);

	//handle_error_code(rc);
	//set_headers(rc);
}

void	HTTPProtocol::handle_method(t_response_creator &r) {
	if (r.req.method == "GET") {
		this->handle_get(r);
	}
	else
		r.err_code = 500;
}

std::string	HTTPProtocol::format_response(t_response &res) {
	return (res.status_line + CRLF + res.headers + CRLF + res.body);
}
