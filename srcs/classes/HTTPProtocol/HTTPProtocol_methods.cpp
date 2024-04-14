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
	//handle login case
	if (r.req.content_is_type("application/json")) {
		std::clog << "json detected, atempting to log in" << "\033[0m\n";//debug
		//not implemented : need access to the Users::handle_user methode
	}
	//check if uri is good
	if (r.req.uri.empty() || r.req.uri[0] != '/') {
		r.err_code = 400;
		return ;
	}
	//optional : check file size, 407 if too large
	if (body_too_large(r.req, r.conf->client_max_body_size)) {
		r.err_code = 413;
		return;
	}
	//check if uri in upload folder (if not, 403)
	if (!path_in_dir(r.req.uri, r.conf->upload_path)) {
		std::cout << "\033[31mERROR:" << r.req.uri << " is not a valid upload uri" << "\033[0m" << std::endl;
		r.err_code = 403;
		return;
	}
	//get the full path of the file
	std::string	full_path = get_full_path_dir(r.req.uri, r.conf);
	if (full_path.empty()) {
		r.err_code = 403;
		return;
	}
	//create the file (if fail, 500)
	std::ofstream	upload_file(full_path.c_str());
	if (!upload_file.is_open()) {
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
	//check if uri is good
	if (r.req.uri.empty() || r.req.uri[0] != '/') {
		r.err_code = 400;
		return ;
	}
	//check if uri in upload folder
	if (!path_in_dir(r.req.uri, r.conf->upload_path)) {
		std::cout << "\033[31m" << r.req.uri << " is not a valid upload uri" << "\033[0m" << std::endl;
		r.err_code = 403;
		return;
	}
	//get file fullpath
	std::string	full_path = get_full_path_file(r.req.uri, r.conf);
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
