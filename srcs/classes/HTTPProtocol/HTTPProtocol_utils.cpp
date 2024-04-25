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

/* function that check the content-type of a request.
 * it will return true if the content-type header contain the string type.
 * it does not check a strict equality, as the field may contain other data too.
 */
bool	t_request::content_is_type(std::string type) {
	if (this->headers.count("content-type") == 0) {
		return false;
	}
	if (this->headers["content-type"][0].find(type) == std::string::npos) {
		return false;
	}
	return true;
}

void	HTTPProtocol::save_user_session(std::string const &path) const {
	this->user_manager.save_sessions(path);
}

void	HTTPProtocol::get_file_type(t_response_creator &r) {
	unsigned long	ext_index = r.file.find_last_of(".");
	if (ext_index != std::string::npos) {
		r.file_type = r.file.substr(ext_index + 1);
	} else {
		r.file_type = "";
	}
}

/* function that check if the body of a request is bigger than the max allowed upload size
 * it first check the content-length header (if it exist)
 * it then check the size() methode of the body, to avoid being fooled by a lie
 * it will return true if either value is bigger than the max size, false else.
 */
bool	HTTPProtocol::body_too_large(t_request& req, size_t size_max) {
	size_t	body_size = 0;
	if (req.headers.count("content-length") > 0) {
		body_size = atol(req.headers["content-length"][0].c_str());
	}
	if (req.body.size() > body_size) {
		body_size = req.body.size();
	}
	if (body_size > size_max) {
		return true;
	} else {
		return false;
	}
}
