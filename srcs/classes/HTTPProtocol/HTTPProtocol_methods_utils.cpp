#include "HTTPProtocol.hpp"

HTTPConfig::t_location	const HTTPProtocol::get_dir_uri(std::string const &uri, HTTPConfig::t_config *conf) {
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

std::string	const HTTPProtocol::get_complete_uri(std::string const &uri, HTTPConfig::t_config *conf) {
	HTTPConfig::t_location	dir = get_dir_uri(uri, conf);
	std::string	file = uri.substr(dir.default_uri.length());

	std::string	filename, filewdir;
	if (file == "/" || file == "") {
		filename = dir.index;
	}
	else
		filename = file;

	if (dir.alias)
		filewdir = dir.replacement + "/" + filename;
	else if (file == "/" || file == "")
		filewdir = dir.replacement + "/" + uri + "/" + filename;
	else
		filewdir = dir.replacement + "/" + uri;

	//std::cout << "FILE = " << filewdir << std::endl;

	return (filewdir);
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
