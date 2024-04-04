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

/* function that check if a given string is the path to a directory
 * it return true if it's a dir, false if it's a normal file or if it does not exist.
 * a directory that exist, but does not have write access, will also return false.
 */
static bool	is_dir(std::string& path) {
	struct stat	filestat;

	if (access(path.c_str(), W_OK)) { //if file/dir does not have write access
		return false;
	}
	if (stat(path.c_str(), &filestat) != 0) {
		return false;
	}
	if S_ISDIR(filestat.st_mode) {
		return true;
	} else {
		return false;
	}
}

/* function that check if a given string is the path to a normal file.
 * it return true if it is an existing file, false if it's a directory or if it does not exist.
 * a file that exist, but without write access, will also return false
 */
static bool	is_reg_file(std::string& path) {
	struct stat	filestat;

	if (access(path.c_str(), W_OK)) { //if file/dir does not have write access
		return false;
	}
	if (stat(path.c_str(), &filestat) != 0) {
		return false;
	}
	if S_ISREG(filestat.st_mode) {
		return true;
	} else {
		return false;
	}
}
/* function that take an URI and a config struct, and return the corresponding path to the directory
 * for each location, it will try to substitute any matching uri by the correspoding path,
 * and check if the directory exist.
 * if several location match, only the first one is returned.
 * if no matches are found, an empty string is returned.
 */
std::string	HTTPProtocol::get_full_path_dir(std::string& uri, HTTPConfig::t_config* conf) {
	std::vector<HTTPConfig::t_location> locs = conf->locations;
	locs.push_back(conf->default_root); //also check default location
	for (size_t i = 0; i < locs.size(); i++) {
		if (uri.rfind(locs[i].default_uri, 0 == std::string::npos)) {
			continue;
		}
		//substitute location for actual path
		std::string full_path = locs[i].replacement + "/"
			+ uri.substr(locs[i].default_uri.size()); //uri with location string removed
		std::string base_path = full_path.substr(0, full_path.find_last_of('/')); //remove file name
		if (is_dir(base_path) && !is_dir(full_path)) {
			return (full_path);
		}
	}
	//if we exited the loop, there is no match, so return error
	return ("");
}

/* function that take an URI and a config struct, and return the corresponding path to an existing file
 * for each location, it will try to substitute any matching uri by the correspoding path,
 * and check if the directory exist.
 * if several location match, only the first one is returned.
 * if no matches are found, an empty string is returned.
 */
std::string	HTTPProtocol::get_full_path_file(std::string& uri, HTTPConfig::t_config* conf) {
	std::vector<HTTPConfig::t_location> locs = conf->locations;
	locs.push_back(conf->default_root); //also check default location
	for (size_t i = 0; i < locs.size(); i++) {
		if (uri.rfind(locs[i].default_uri, 0 == std::string::npos)) {
			continue;
		}
		std::string full_path = locs[i].replacement + "/"
			+ uri.substr(locs[i].default_uri.size()); //remove location string
		if (is_reg_file(full_path)) {
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

