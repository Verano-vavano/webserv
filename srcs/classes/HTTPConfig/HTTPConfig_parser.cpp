#include "HTTPConfig.hpp"

int HTTPConfig::parse_infile(std::ifstream &f) {
	char                    buffer[BUFFER_SIZE];
	std::streamsize         bytes;
	HTTPConfig::t_parser    opt;

	opt.options = 0;
	opt.line = 1;
	opt.in_http = false;
	opt.current_serv = &(this->default_config);
	do {
		f.read(buffer, BUFFER_SIZE - 1);
		bytes = f.gcount();
		buffer[bytes] = '\0';
		if (this->understand_the_line(buffer, opt) == 1)
			return (1);
	} while (bytes == BUFFER_SIZE - 1);

	if (!opt.blocks.empty()) {
		std::cerr << "[WARNING] Blocks not closed" << std::endl;
		if (this->bitmask_warning(opt.options)) { return (1); }
	}
	return (0);
}

// -1 = Continue
// 0 = OK
// 1 = Error parsing && ERROR_STOP on
int HTTPConfig::understand_the_line(char *buffer, HTTPConfig::t_parser &opt) {
	std::pair<char, int>    delim;
	std::string             cut;
	int						ret;

	while (true) {
		delim = this->search_delim(buffer, opt);
		if (!delim.first)
			return (-1);
		buffer[delim.second] = '\0';
		cut = this->trim_buffer(buffer);
		if (delim.first == '}') {
			if (opt.blocks.size() == 0) {
				std::cerr << "[ERROR] Extra '}' at line " << opt.line << std::endl;
				if (opt.options & O_ERROR_STOP)
					return (1);
			}
			else if (cut != "") {
				std::cerr << "[ERROR] Missing separator near line " << opt.line << std::endl;
				if (opt.options & O_ERROR_STOP)
					return (1);
			}
			else {
				opt.blocks.pop();
				if (opt.blocks.empty())
					opt.in_http = false;
			}
		}
		else if (delim.first == '{') {
			ret = this->set_block(cut, opt);
			opt.blocks.push(cut);
		}
		else { ret = understand_the_cut(cut, opt); }
		buffer += delim.second + 1;
		if (ret == 1)
			return (ret);
	}
	return (ret);
}

int	HTTPConfig::understand_the_cut(std::string & cut, HTTPConfig::t_parser &opt) {
	if (cut.substr(0, 6) == "DEFINE") {
		return (this->set_define(cut, opt));
	}
	else {
		if (!opt.in_http) {
			std::cerr << "[WARNING] Not DEFINE not in a HTTP block at line " << opt.line << std::endl;
			if (this->bitmask_warning(opt.options)) { return (1); }
		}
		return (this->set_other(cut, opt));
	}
	return (0);
}


// RETURN 2 : skip the block
// RETURN 1 : quit
// RETURN 0 : all good ma man
int	HTTPConfig::set_block(std::string & cut, HTTPConfig::t_parser &opt) {
	std::vector<std::string>	split;
	split_cut(split, cut);

	std::string	method = split.front();

	// LOCATION
	if (method == "location") {
		t_location	tmp;
		if (split.size() == 1) {
			std::cerr << "[ERROR] No URI for location at line " << opt.line << std::endl;
			return (2 - (opt.options & O_ERROR_STOP));
		}
		else if (split.size() > 2) {
			std::cerr << "[WARNING] Multiple URI for location (not supported) at line " << opt.line << std::endl;
			if (HTTPConfig::bitmask_warning(opt.options)) { return (1); }
		}
		tmp.default_uri = split[1];
		tmp.replacement = "";
		opt.current_serv->locations.push_back(tmp);
	}

	// HTTP
	else if (method == "http") {
		if (!opt.blocks.empty()) {
			std::cerr << "[FATAL ERROR] http block not in global scope at line " << opt.line << std::endl;
			return (1);
		}
		opt.in_http = true;
	}
	return (0);
}


int	HTTPConfig::set_define(std::string & cut, HTTPConfig::t_parser &opt) {
	if (opt.blocks.size() != 0) {
		std::cerr << "[ERROR] Invalid define location at line " << opt.line << std::endl;
		if (HTTPConfig::bitmask_warning(opt.options)) { return (1); }
	}

	int	start = 6;
	for (; isspace(cut[start]); start++) {}
	std::string	method = cut.substr(start);

	if (method == "SPACE_MODE")
		opt.options |= O_SPACE_MODE;
	else if (method == "ERROR_STOP")
		opt.options |= O_ERROR_STOP;
	else if (method == "WARNING_AS_ERROR")
		opt.options |= O_WARNING_AS_ERROR;
	else {
		std::cerr << "[ERROR] Unknown define at line " << opt.line << std::endl;
		return (opt.options & O_ERROR_STOP);
	}
	return (0);
}


int	HTTPConfig::set_other(std::string & cut, HTTPConfig::t_parser &opt) {
	std::vector<std::string>	split;
	split_cut(split, cut);

	std::string	method = split.front();

	// LOCATION METHODS
	if (method == "root" || method == "alias" || method == "index") {
		if (opt.blocks.top().substr(0, 8) != "location") {
			std::cerr << "[ERROR] " << method << " outside of 'location' block at line " << opt.line << std::endl;
			if (opt.options & O_ERROR_STOP) { return (1); }
		}
		else {
			HTTPConfig::t_location	*tmp = &(opt.current_serv->locations.back());
			if (method == "root" || method == "alias") {
				if (tmp->replacement != "") {
					std::cerr << "[WARNING] Overwriting already existing alias at line " << opt.line << std::endl;
					if (HTTPConfig::bitmask_warning(opt.options)) { return (1); }
				}
				if (split.size() == 1) {
					std::cerr << "[WARNING] No location for a uri at line " << opt.line << std::endl;
					return (HTTPConfig::bitmask_warning(opt.options));
				}
				if (split.size() != 2) {
					std::cerr << "[WARNING] Multiple locations for a uri at line " << opt.line << std::endl;
					if (HTTPConfig::bitmask_warning(opt.options)) { return (1); }
				}
				tmp->replacement = split[1];
				tmp->alias = (method == "alias");
			}
		}
	}
	return (0);
}


std::pair<char, int>    HTTPConfig::search_delim(std::string const buffer, HTTPConfig::t_parser &opt) {
	std::pair<char, int>    ret;

	ret.first = 0;
	for (int i = 0; buffer[i]; i++) {
		if (buffer[i] == '{' || buffer[i] == ';' || buffer[i] == '}') {
			ret.first = buffer[i];
			ret.second = i;
			break ;
		}
		else if (buffer[i] == '\n')
			opt.line++;
	}
	return (ret);
}


std::string HTTPConfig::trim_buffer(char *buffer) {
	int start = 0;
	int end = strlen(buffer) - 1;

	for (; buffer[start] && isspace(buffer[start]); start++) {}
	for (; end > 0 && isspace(buffer[end]); end--) {}
	buffer[end + 1] = '\0';
	return (buffer + start);
}


void	HTTPConfig::split_cut(std::vector<std::string> &s, std::string const & cut) {
	size_t	i = 0;
	size_t	new_i;

	do {
		new_i = cut.find_first_of(ISSPACE, i);
		s.push_back(cut.substr(i, new_i));
		i = cut.find_first_not_of(ISSPACE, new_i);
	} while (i != std::string::npos);

	return ;
}


bool inline	HTTPConfig::bitmask_warning(int mask) {
	return ((mask & O_WARNING_AS_ERROR) && (mask & O_ERROR_STOP));
}
