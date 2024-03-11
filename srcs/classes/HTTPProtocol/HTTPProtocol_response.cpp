#include "HTTPProtocol.hpp"

void	HTTPProtocol::create_response(t_response_creator &rc) {
	rc.res.err_code = 200;
	rc.res.status_line = "HTTP/1.1";

	this->handle_method(rc);
}

void	HTTPProtocol::handle_method(t_response &res, t_request &req) {
	if (req.method == "GET") {
		this->handle_get(res, req);
	}
	else {
		res.err_code = 500;
	}
}

std::string	HTTPProtocol::format_response(t_response &res) {
	return (res.status_line + CRLF + res.headers + CRLF + res.body);
}
