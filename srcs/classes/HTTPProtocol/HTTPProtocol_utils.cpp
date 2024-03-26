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

bool	HTTPProtocol::is_wildcard_match(std::string const & input, std::string const & match) {
	std::string	next_pattern;
	unsigned long	i_index = 0;
	unsigned long	i_end_pattern;

	for (unsigned long i_match = 0; i_match < match.size(); i_match++) {
		if (match[i_match] == '*') { continue ; }
		for (i_end_pattern = i_match; i_end_pattern != match.size() && match[i_end_pattern] != '*'; i_end_pattern++) { }
		next_pattern = match.substr(i_match, i_end_pattern - i_match);
		std::cout << next_pattern << std::endl;
		// FIND PATTERN BEGIN
		if (i_match == 0 && input.substr(0, next_pattern.size()) != next_pattern) { return (false); }
		else if (i_end_pattern == match.size()) {
			// FIND PATTERN END
			i_index = input.find_last_of(next_pattern);
			if (i_index != input.size() - 1) { return (false); }
		}
		else if (i_match != 0) {
			// FIND PATTERN MIDDLE
			i_index = input.find(next_pattern, i_index);
			if (i_index > input.size()) { return (false); }
			i_index += next_pattern.size();
		}
		i_match = i_end_pattern;
	}
	return (true);
}

bool	HTTPProtocol::is_directory(std::string const & file) {
	struct stat file_stat;
	if (stat(file.c_str(), &file_stat) == 0) {
		return (S_ISDIR(file_stat.st_mode));
	}
	return (false);
}
