#include "HTTPProtocol.hpp"

void	HTTPProtocol::create_response(t_response_creator &rc) {
	rc.err_code = 200;
	rc.res.status_line = "";
	rc.res.headers = "";
	if (rc.req.method != "POST")
		rc.res.body = "";

	this->handle_method(rc); // Gets body from request method
	rc.file_type = get_mime_type(rc.conf, rc.file_type);
	if (rc.err_code == 200)
		this->check_type(rc); // Checks if file type matches Accept header
	this->handle_error_code(rc); // Gets body if error
	if (rc.req.method == "POST") {
		rc.file_type = "application/json; charset=UTF-8";
	}
	this->set_headers(rc); // Sets headers wow
	rc.res.status_line = "HTTP/1.1 " + this->get_error_tag(rc.err_code);
}

void	HTTPProtocol::handle_method(t_response_creator &r) {
	if (r.req.method == "GET") {
		this->handle_get(r);
	}
	else if (r.req.method != "POST")
		r.err_code = 501;
}

std::string	HTTPProtocol::format_response(t_response &res) {
	return (res.status_line + CRLF + res.headers + CRLF + res.body);
}
