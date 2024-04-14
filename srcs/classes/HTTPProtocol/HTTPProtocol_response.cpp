#include "HTTPProtocol.hpp"

void	HTTPProtocol::create_response(t_response_creator &rc) {
	rc.err_code = 200;
	rc.res.status_line = "";
	rc.res.headers = "";
	if (rc.req.method != "POST")
		rc.res.body = "";

	this->handle_method(rc); // Gets body from request method
	rc.file_type = get_mime_type(rc.conf, rc.file_type);
	if (rc.req.http_version != "HTTP/1.1") {
		rc.err_code = 505;
	} else if (rc.err_code == 200)
		this->check_type(rc); // Checks if file type matches Accept header
	if (rc.req.method == "POST") {
		rc.file_type = "application/json; charset=UTF-8";
	}
	this->handle_error_code(rc); // Gets body if error
	this->set_headers(rc); // Sets headers wow
	rc.res.status_line = "HTTP/1.1 " + this->get_error_tag(rc.err_code);
}

void	HTTPProtocol::handle_method(t_response_creator &r) {
	r.better_uri = this->remove_useless_slashes(r.req.uri);
	if (r.better_uri[r.better_uri.size() - 1] != '/') { r.better_uri += "/"; }
	r.location = &(get_dir_uri(r.better_uri, r.conf));
	std::map<std::string, bool>::const_iterator	finder = r.location->methods.find(r.req.method);
	if (finder != r.location->methods.end() && !finder->second) {
		r.err_code = 405;
		return ;
	}

	if (r.req.method == "GET") {
		this->handle_get(r);
	} else if (r.req.method == "POST") {
		this->handle_post(r);
	} else if (r.req.method == "DELETE") {
		this->handle_delete(r);
	} else {
		r.err_code = 501;
	}
}

std::string	HTTPProtocol::format_response(t_response &res) {
	return (res.status_line + CRLF + res.headers + CRLF + res.body);
}
