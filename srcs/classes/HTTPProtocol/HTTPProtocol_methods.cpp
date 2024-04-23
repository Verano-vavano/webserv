#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_get(t_response_creator &r) {
	std::string	&uri = r.req.uri;
	if (uri.empty() || uri[0] != '/') {
		r.err_code = 400;
		return ;
	}

	this->get_body(uri, r, -1);
}

void	HTTPProtocol::handle_post(t_response_creator &r) {
	//handle login case -------------------------------------------------------- does not work, r.location->post_func is NONE
	if (r.location->post_func == "CLIENT_MANAGER") {
		this->user_manager.handle_post(r);
		if (r.err_code == 200)
			r.file_type = "application/json; charset=UTF-8";
		return ;
	}
	//optional : check file size, 407 if too large
	if (body_too_large(r.req, r.conf->client_max_body_size)) {
		r.err_code = 413;
		return;
	}
	//check if the request is an upload or an exec
	if (r.req.content_is_type("application/x-www-form-urlencoded")) {
		if (access(r.file.c_str(), X_OK)) {
			r.err_code = 404;
			return;
		}
		//exec CGI
		cgi(r); //not sure if it work yet
		return;
	}
	//create the file (if fail, 500)
	std::ofstream	upload_file(r.file.c_str());
	if (upload_file.is_open()) {
		r.err_code = 500;
		return;
	}
	//write body to file (if fail, 500)
	upload_file << r.req.body;
	if (upload_file.fail()) {
		r.err_code = 500;
		return;
	}
	//close file
	upload_file.close();
	r.err_code = 200;
	return;
}

void	HTTPProtocol::handle_delete(t_response_creator &r) {
	if (r.location->del_func == "CLIENT_MANAGER") {
		this->user_manager.handle_del(r);
		return ;
	}
	//check if uri is good
	if (r.req.uri.empty() || r.req.uri[0] != '/') {
		r.err_code = 400;
		return ;
	}
	//get file fullpath
	std::string	full_path = get_full_path_file(r.req.uri, r.conf, W_OK);
	if (full_path.empty()) {
		r.err_code = 404;
		return;
	}
	//delete the file
	if (std::remove(full_path.c_str())) {
		std::cout << "\033[31m ERROR: can't remove " << r.req.uri << "(aka " << full_path << ")\033[0m" << std::endl;
		r.err_code = 500;
		return;
	}
	r.err_code = 200;
	return;
}
