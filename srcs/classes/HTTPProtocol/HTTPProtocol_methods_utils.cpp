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
	HTTPConfig::t_location const	&dir = get_dir_uri(better_uri, conf);
	std::string	file = better_uri.substr(dir.default_uri.length());

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

	if (dir.alias)
		ret.file = dir.replacement + "/" + filename;
	else if (file == "/" || file == "")
		ret.file = dir.replacement + "/" + better_uri + "/" + filename;
	else
		ret.file = dir.replacement + "/" + better_uri;

	return (ret);
}

void	HTTPProtocol::get_body(std::string const &uri, t_response_creator &r, int change) {
	t_uri_cgi	full_uri = this->get_complete_uri(uri, r.conf);
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
			if (is_wildcard_match(uri, *it)) {
				exec_cgi(full_uri.file, NULL, r);
				return ;
			}
		}
	}
	if (change != -1)
		r.err_code = change;
	this->read_entire_file(r.res.body, file);
	return ;
}

std::string	const HTTPProtocol::get_mime_type(HTTPConfig::t_config *config, std::string &file_type) {
	HTTPConfig::t_type	t_list = config->types;
	if (t_list.find(file_type) != t_list.end()) {
		return (t_list[file_type]);
	}
	else if (file_type == "html") { return (HTML); }
	else if (file_type == "css") { return (CSS); }
	else if (file_type == "js") { return (JS); }
	else if (file_type == "webp") { return (WEBP); }
	return (config->default_type);
}
