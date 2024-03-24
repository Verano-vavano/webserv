#include "HTTPConfig.hpp"

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

void	HTTPConfig::skip_block(std::string & buffer, int start) {
	for (; buffer[start] != '}'; start++) {}
	buffer = buffer.substr(start + 1);
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
