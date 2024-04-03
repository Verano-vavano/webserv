#include "HTTPProtocol.hpp"

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

/* function that take an URI and a config struct, and return the corresponding path
 * for each location, it will try to substitute any matching uri by the correspoding path,
 * and check if the directory exist.
 * if several location match, only the first one is returned.
 * if no matches are found, an empty string is returned.
 */
std::string	HTTPProtocol::get_full_path(std::string& uri, HTTPConfig::t_config* conf) {
	std::vector<HTTPConfig::t_location> locs = conf->locations;
	locs.push_back(conf->default_root); //also check default location
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
	//if we exited the loop, there is no match, so return error
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

