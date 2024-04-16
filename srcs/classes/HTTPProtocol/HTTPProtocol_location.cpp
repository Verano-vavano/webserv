#include "HTTPProtocol.hpp"

HTTPConfig::t_location	&HTTPProtocol::get_dir_uri(std::string const &uri, HTTPConfig::t_config *conf) {
	unsigned int									last_i = uri.size();
	std::vector<HTTPConfig::t_location>::iterator	finder;
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

std::string	const HTTPProtocol::get_complete_uri(t_response_creator const &r) {
	std::string	better_uri = this->remove_useless_slashes(r.req.uri);
	std::string	uri_with_slash = r.better_uri;
	HTTPConfig::t_location const	*dir = r.location;
	std::string	file = uri_with_slash.substr(dir->default_uri.length());

	std::string	filename;
	if (dir->index.size()) {
		filename = dir->index;
	}
	else {
		filename = file;
	}

	if (dir->alias) {
		file = dir->replacement + "/" + filename;
	} else if (file == "/" || file == "") {
		file = dir->replacement + "/" + better_uri + "/" + filename;
	} else {
		file = dir->replacement + "/" + better_uri;
	}

	return (file);
}
