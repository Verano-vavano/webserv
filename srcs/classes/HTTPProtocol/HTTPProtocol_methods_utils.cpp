#include "HTTPProtocol.hpp"

HTTPConfig::t_location	const HTTPProtocol::get_dir_uri(std::string &uri, HTTPConfig::t_config *conf) {
	unsigned int									last_i = uri.size();
	std::vector<HTTPConfig::t_location>::const_iterator	finder;
	std::string							sub;
	bool								found = false;

	while (!found && last_i != std::string::npos && last_i != 0) {
		last_i = uri.find_last_of("/", last_i);
		std::cout << last_i << std::endl;
		sub = uri.substr(0, last_i + 1);
		for (finder = conf->locations.begin(); finder != conf->locations.end(); finder++) {
			if (finder->default_uri == sub) { found = true; break ; }
		last_i--;
		}
	}

	if (found)
		return (*finder);

	HTTPConfig::t_location	default_loc;
	return (default_loc);
}
