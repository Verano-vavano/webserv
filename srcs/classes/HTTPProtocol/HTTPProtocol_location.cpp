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

void	HTTPProtocol::get_complete_uri(t_response_creator &r, std::string const &uri) {
	std::string	better_uri = remove_useless_slashes(uri);
	std::string	uri_with_slash = r.better_uri;
	HTTPConfig::t_location *dir = r.location;
	std::string	file = uri_with_slash.substr(dir->default_uri.length());

	std::string	filename;
	if (dir->index.size()) {
		filename = dir->index;
	}
	else {
		filename = file;
	}

	if (dir->alias) {
		r.file_wo_index = dir->replacement;
	} else {
		r.file_wo_index = dir->replacement + "/" + better_uri;
	}

	if (file == "/" || file == "") {
		r.file = r.file_wo_index + "/" + filename;
	} else {
		r.file = r.file_wo_index;
	}
}
