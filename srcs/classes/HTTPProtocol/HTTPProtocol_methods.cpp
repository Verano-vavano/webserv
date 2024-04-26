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
	} else if (r.location->post_func == "UPLOAD_MANAGER") {
		std::cout << "Hey now, youre an all star, get the game on, go play. hey now, youre a rockstar, get the show on get paid. and all that glitter is gold. only shooting stars break the moooooold" << std::endl;
		//check if a forbiden file already exist
		if (!access(r.file.c_str(), F_OK) && access(r.file.c_str(), W_OK)) { //if file exist without write access
			r.err_code = 403;
			return;
		}
		//create the file (if fail, 500)
		std::cout << r.file << std::endl;
		std::ofstream	upload_file(r.file.c_str());
		if (!upload_file.is_open()) {
			std::cout << "its a cool place" << std::endl;
			r.err_code = 500;
			return;
		}
		//write body to file (if fail, 500)
		upload_file << r.req.body;
		if (upload_file.fail()) {
			std::cout << "shrekk" << std::endl;
			r.err_code = 500;
			return;
		}
		//close file
		upload_file.close();
		r.err_code = 200;
		r.res.body = "Succesfully created the file";
		return;
	} else {
		r.err_code = 403;
	}
}

void	HTTPProtocol::handle_delete(t_response_creator &r) {
	if (r.location->del_func == "CLIENT_MANAGER") {
		this->user_manager.handle_del(r);
		return ;
	} else if (r.location->del_func == "UPLOAD_MANAGER") {
		//check if file is removable :
		if (access(r.file.c_str(), W_OK)) {
			r.err_code = access(r.file.c_str(), F_OK)?404:403; //404 if no file, 403 if exist but wrong rights
			return;
		}
		//delete the file
		if (std::remove(r.file.c_str())) { //remove, and check if it worked
			r.err_code = is_directory(r.file)?403:500; //return 403 if it's a directory, 500 else
			return;
		}
		r.err_code = 200;
		r.res.body = "Succesfully removed the file";
		return;
	} else {
		r.err_code = 403;
	}
}
