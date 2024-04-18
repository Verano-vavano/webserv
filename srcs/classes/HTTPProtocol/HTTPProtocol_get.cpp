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

bool	HTTPProtocol::get_body(std::string const &uri, t_response_creator &r, int change) {
	std::cout << *(r.location) << std::endl;
	if (this->is_directory(r.file_wo_index)) {
		if (r.location->dir_listing) {
			this->directory_listing(r, r.file_wo_index, uri);
			r.file = "";
			return (false);
		} else if (r.file_wo_index == r.file) {
			r.err_code = 403;
			return (false);
		}
	}

	std::ifstream	file((r.conf->path + r.file).c_str());
	if (!file || !file.good()) {
		r.err_code = 404;
		return (false);
	}

	if (change != -1)
		r.err_code = change;

	if (r.conf->chunked_transfer_encoding) {
		r.file = r.file;
		return (true);
	}

	this->read_entire_file(r.res.body, file);
	return (true);
}
