#include "HTTPProtocol.hpp"

void	HTTPProtocol::directory_listing(t_response_creator &r, std::string const & dir, std::string const &uri) {
	DIR*	directory = opendir(dir.c_str());
	if (!directory) {
		r.err_code = 403;
		return ;
	}

	r.file_type = "html";
	std::string & body = r.res.body;
	std::string	better_dir = HTTPProtocol::remove_useless_slashes(dir);

	body = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"></head><body><h1>";
	body += better_dir + " listing</h1><ul>";

	struct dirent*	entry;
	std::string	file;
	std::string	complete_uri = uri;
	if (uri.size() && uri.at(uri.size() - 1) != '/') {
		complete_uri += "/";
	}
	while ((entry = readdir(directory))) {
		file = entry->d_name;
		// Maybe add a slash if there is not
		body += "<a href=" + complete_uri + file + "><li>" + file + "</a></li>";
	}

	body += "</ul></body></html>";

	closedir(directory);
	return ;
}

void	HTTPProtocol::get_body(std::string const &uri, t_response_creator &r, int change) {
	std::string	full_uri = this->get_complete_uri(r);
	if (this->is_directory(full_uri)) {
		if (r.location->dir_listing)
			this->directory_listing(r, full_uri, uri);
		else
			r.err_code = 403;
		return ;
	}

	std::ifstream	file((r.conf->path + full_uri).c_str());
	if (!file || !file.good()) {
		r.err_code = 404;
		return ;
	}

	if (change != -1)
		r.err_code = change;

	if (r.conf->chunked_transfer_encoding) {
		r.file = full_uri;
		return ;
	}

	this->read_entire_file(r.res.body, file);
	return ;
}
