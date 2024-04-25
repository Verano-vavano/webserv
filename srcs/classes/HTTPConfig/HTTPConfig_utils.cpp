#include "HTTPConfig.hpp"

std::pair<char, int>	HTTPConfig::search_delim(std::string const buffer, HTTPConfig::t_parser &opt) {
	std::pair<char, int>	ret;

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


// trim the whitespaces at the start and at the end
std::string HTTPConfig::trim_buffer(char *buffer) {
	int start = 0;
	int end = strlen(buffer) - 1;

	for (; buffer[start] && isspace(buffer[start]); start++) {}
	for (; end > 0 && isspace(buffer[end]); end--) {}
	buffer[end + 1] = '\0';
	return (buffer + start);
}

std::string HTTPConfig::trim_buffer(std::string const & buffer) {
	int start = 0;
	int end = buffer.size() - 1;

	for (; buffer[start] && isspace(buffer[start]); start++) {}
	for (; end > 0 && isspace(buffer[end]); end--) {}
	return (buffer.substr(start, end - start + 1));
}


void	HTTPConfig::split_cut(std::vector<std::string> &s, std::string const & cut) {
	size_t	i = 0;
	size_t	new_i;

	do {
		new_i = cut.find_first_of(ISSPACE, i);
		s.push_back(cut.substr(i, new_i - i));
		i = cut.find_first_not_of(ISSPACE, new_i);
	} while (i != std::string::npos);

	return ;
}

void	HTTPConfig::skip_block(std::string & buffer, unsigned int start) {
	for (; start < buffer.size() && buffer[start] != '}'; start++) {}
	if (start == buffer.size()) { buffer = ""; }
	else { buffer = buffer.substr(start + 1); }
}

// checks if s is in the NULL-terminated va_list
bool	HTTPConfig::in(std::string const s, ...) {
	va_list	vargs;
	char	*to_cmp;

	va_start(vargs, s);

	to_cmp = va_arg(vargs, char*);
	while (to_cmp) {
		if (s == to_cmp)
			return (true);
		to_cmp = va_arg(vargs, char*);
	}
	return (false);
}

long	HTTPConfig::translate_time(std::string arg) {
	int	end = 0;
	for (; arg[end] && isdigit(arg[end]); end++) {}

	long	ret = std::atol(arg.c_str());

	if (ret < 0) { return (-1); }

	switch (arg[end]) {
		case 'm':
			if (ret > std::numeric_limits<long>::max() / 60) {
				return (-1);
			}
			ret *= 60;
			break ;
		case 'h':
			if (ret > std::numeric_limits<long>::max() / 360) {
				return (-1);
			}
			ret *= 360;
			break ;
	}

	return (ret);
}

// For the TOGGLE_BOOL define, we use one of the properties of the NAND gate
// 1 NAND 1 = 0, 0 NAND 1 = 1 so if B = 1, A is 'switched'
// 1 NAND 0 = 1, 0 NAND 0 = 1 so if B = 0, A is true
bool	HTTPConfig::boolean_switch(bool &var, t_parser const & opt, std::vector<std::string> const & split) {
	bool	on, easy;
	easy = (split.size() != 1);
	if (!easy) { on = (opt.options & O_TOGGLE_BOOL); }
	else {
		if (split.size() > 2 && HTTPConfig::warning("More than one argument to boolean method " + split[0], opt.line, opt.options)) { return (1); }
		on = (split[1] == "on");
	}
	var = (easy ? on : !(var & on));
	return (0);
}


std::string	HTTPConfig::to_upper(std::string const & old) {
	std::string	ret = old;
	for (unsigned int i = 0; i < ret.size(); i++) {
		if (islower(ret[i])) { ret[i] = toupper(ret[i]); }
	}
	return (ret);
}

bool	HTTPConfig::isallnum(std::string const & str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
		if (!std::isdigit(*it)) { return (false);}
	}
	return (true);
}

HTTPConfig::t_location	*HTTPConfig::get_cur_location(t_parser &opt) {
	if (opt.blocks.size() && opt.blocks.top().size() >= 8 && opt.blocks.top().substr(0, 8) == "location") {
		return (opt.current_location);
	} else {
		return (&(opt.current_serv->default_root));
	}
}

int	HTTPConfig::split_server_name(std::string const & str, std::pair<std::string, int> &serv) {
	size_t	index = str.find(':');
	bool	both = (index != std::string::npos);

	serv.first = DEFAULT_NAME;
	serv.second = DEFAULT_PORT;

	if (!both) {
		if (isdigit(str[0])) {
			serv.second = atoi(str.c_str());
		} else {
			serv.first = str;
		}
	} else {
		size_t	sub_index = str.find(':', index + 1);
		serv.first = str.substr(0, index);
		serv.second = atoi(str.substr(index + 1, sub_index).c_str());
	}
	return (0);
}

HTTPConfig::t_config	*HTTPConfig::get_config(std::pair<std::string, int> const &serv) {
	for (unsigned long l = 0; l < this->servers.size(); l++) {
		if (serv.second == this->servers[l].port && serv.first == this->servers[l].server_name) {
			return (&(this->servers[l]));
		}
	}
	return (NULL);
}

HTTPConfig::t_location	*HTTPConfig::get_location(t_config &serv, std::string const default_uri) {
	for (unsigned long l = 0; l < serv.locations.size(); l++) {
		if (default_uri == serv.locations[l].default_uri) {
			return (&(serv.locations[l]));
		}
	}
	return (NULL);
}
