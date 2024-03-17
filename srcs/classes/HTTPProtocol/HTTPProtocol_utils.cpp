#include "HTTPProtocol.hpp"

void	HTTPProtocol::read_entire_file(std::string &buf, std::ifstream &file) {
	std::stringstream	buffer;

	buffer << file.rdbuf();
	buf = buffer.str();
}

std::string	HTTPProtocol::remove_useless_slashes(std::string const &uri) {
	std::string	ret;
	bool		slash = false;

	for (std::string::const_iterator it = uri.begin(); it != uri.end(); it++) {
		if (*it == '/') {
			if (slash) { continue ; }
			slash = true;
		} else if (slash) { slash = false; }
		ret.push_back(*it);
	}
	return (ret);
}
