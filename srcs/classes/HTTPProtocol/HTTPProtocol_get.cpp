#include "HTTPProtocol.hpp"

HTTPConfig::t_location	const &HTTPProtocol::get_dir_uri(std::string const &uri, HTTPConfig::t_config *conf) {
	unsigned int									last_i = uri.size();
	std::vector<HTTPConfig::t_location>::const_iterator	finder;
	std::string							sub;
	bool								found = false;

	while (!found && last_i <= uri.size() && last_i != 0) {
		last_i = uri.find_last_of("/", last_i);
		sub = uri.substr(0, last_i + 1);
		for (finder = conf->locations.begin(); finder != conf->locations.end(); finder++) {
			if (finder->default_uri == sub) { found = true; break ; }
		}
		last_i--;
	}

	if (found)
		return (*finder);

	return (conf->default_root);
}

t_uri_cgi	const HTTPProtocol::get_complete_uri(std::string const &uri, HTTPConfig::t_config *conf) {
	std::string	better_uri = remove_useless_slashes(uri);
	std::string	uri_with_slash = better_uri;
	if (uri_with_slash[uri_with_slash.size() - 1] != '/') { uri_with_slash += "/"; }
	HTTPConfig::t_location const	&dir = get_dir_uri(uri_with_slash, conf);
	std::string	file = uri_with_slash.substr(dir.default_uri.length());

	t_uri_cgi ret;
	ret.cgi = NULL;

	std::string	filename;
	if (dir.index.size()) {
		filename = dir.index;
	}
	else {
		filename = file;
		ret.cgi = &dir.cgi;
	}

	if (dir.alias) {
		ret.file = dir.replacement + "/" + filename;
	} else if (file == "/" || file == "") {
		ret.file = dir.replacement + "/" + better_uri + "/" + filename;
	} else {
		ret.file = dir.replacement + "/" + better_uri;
	}

	ret.dir_listing = dir.dir_listing;

	return (ret);
}

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
	t_uri_cgi	full_uri = this->get_complete_uri(uri, r.conf);
	if (this->is_directory(full_uri.file)) {
		if (full_uri.dir_listing)
			this->directory_listing(r, full_uri.file, uri);
		else
			r.err_code = 403;
		return ;
	}

	std::ifstream	file((r.conf->path + full_uri.file).c_str());
	if (!file || !file.good()) {
		r.err_code = 404;
		return ;	
	}

	unsigned long	ext_index = full_uri.file.find_last_of(".");
	if (ext_index != std::string::npos) {
		r.file_type = full_uri.file.substr(ext_index + 1);
	} else {
		r.file_type = "";
	}
	if (full_uri.cgi != NULL) {
		HTTPConfig::t_cgi const *cgi = full_uri.cgi;
		std::map<std::string, std::string>::const_iterator int_iter = cgi->cgi_interpreter.find("." + r.file_type);
		if (int_iter != cgi->cgi_interpreter.end()) {
			std::string	interpreter = int_iter->second;
			if (exec_cgi(full_uri.file, &interpreter, r) == 0) { return ; }
		}
		for (std::set<std::string>::const_iterator it = cgi->cgi_exec.begin(); it != cgi->cgi_exec.end(); it++) {
			if (is_wildcard_match(uri, *it) && exec_cgi(full_uri.file, NULL, r) == 0) { return ; }
		}
	}
	if (change != -1)
		r.err_code = change;

	if (r.conf->chunked_transfer_encoding) {
		r.file = full_uri.file;
		return ;
	}

	this->read_entire_file(r.res.body, file);
	return ;
}

bool	HTTPProtocol::path_in_dir(std::string& uri, std::vector<std::string>& allowed) {
	//fill path allowed (until parsing and default values are completed)  //TMP
	if (allowed.empty()) {
		allowed.push_back("/uploaded");
		allowed.push_back("/images");
	}//end tmp
	for (size_t i = 0 ; i < allowed.size(); i++) {
		if (uri.rfind(allowed[i], 0) != std::string::npos) {
			return true;
		}
	}
	return false;
}

static std::string	get_base_path(std::string full_path) {
	size_t	sep_pos;

	sep_pos = full_path.find_last_of('/');
	return full_path.substr(0, sep_pos);
}

// for each location :
	// check if uri start with location.default_uri (if not, continue)
	// substitue default uri with replacement in uri
	// temporarily remove basename from path
	// check if path exist and is a folder
		// if it does, return path + basename
// same thing with default location
// if still no match, return empty string;
std::string	HTTPProtocol::get_full_path(std::string& uri, HTTPConfig::t_config* conf) {
	std::vector<HTTPConfig::t_location> locs = conf->locations;
	locs.push_back(conf->default_root);
	for (size_t i = 0; i < locs.size(); i++) {
		if (uri.rfind(locs[i].default_uri, 0 == std::string::npos)) {
			continue;
		}
		std::string full_path = locs[i].replacement + "/" + uri;
		std::string base_path = get_base_path(full_path);
		if (!access(base_path.c_str(), W_OK)) {
			return (full_path);
		}
	}

	return ("");
}

bool	HTTPProtocol::body_too_large(t_request& req, size_t size_max) {
	size_t	body_size = 0;
	if (req.headers.count("content-length") > 0) {
		body_size = atol(req.headers["content-length"][0].c_str());
	}
	if (req.body.size() > body_size) {
		std::cout << "\033[33m" "WARNING: client gave a wrong body size : " << body_size << "instead of " << req.body.size() << "\033[0m" << std::endl;
		body_size = req.body.size();
	}
	if (body_size > size_max) {
		return true;
	} else {
		return false;
	}
}
