#include "HTTPConfig.hpp"

bool	HTTPConfig::parse_infile(std::ifstream &f, bool space_mode) {
	char                    buffer[BUFFER_SIZE];
	std::string				line;
	std::string				temp = "";
	std::streamsize         bytes;
	HTTPConfig::t_parser    opt;

	if (BUFFER_SIZE < 2) {
		std::cerr << "Buffer size too small (check HTTPConfig.hpp or use custom flag -D to define a new one)" << std::endl;
		return (1);
	}

	if (space_mode) { std::getline(f, line); } // getline dans le vide pour oublier premiere ligne
	opt.options = space_mode * O_SPACE_MODE;
	opt.line = 1;
	opt.in_http = false;
	opt.current_serv = &(this->default_config);
	bytes = BUFFER_SIZE - 1;
	do {
		if (opt.options & O_SPACE_MODE) {
			std::getline(f, line);
			opt.line++;
			if (f.eof()) { break ; }
			if (this->understand_the_line(temp + line, temp, opt) == 1)
				return (1);
		} else {
			f.read(buffer, BUFFER_SIZE - 1);
			bytes = f.gcount();
			buffer[bytes] = '\0';
			if (this->understand_the_line(temp + buffer, temp, opt) == 1)
				return (1);
		}
	} while (bytes == BUFFER_SIZE - 1);

	if (!opt.blocks.empty() && HTTPConfig::warning("Blocks not closed", 0, opt.options)) { return (1); }
	return (0);
}

// -1 = Continue
// 0 = OK
// 1 = Error parsing && ERROR_STOP on
int HTTPConfig::understand_the_line(std::string buffer, std::string & temp, HTTPConfig::t_parser &opt) {
	std::string				cmd;
	std::pair<char, int>    delim;
	std::string             cut;
	int						ret;

	temp = "";
	while (true) {
		delim = this->search_delim(buffer, opt);
		if (!delim.first && !(opt.options & O_SPACE_MODE)) {
			// if no delim, then we haven't read enough
			std::cout << "Early exit at : " << buffer << std::endl;
			temp = buffer;
			return (-1);
		}
		else if (!delim.first) {
			// we in space mode
			delim.first = '\n';
			delim.second = buffer.size();
			cmd = buffer;
		}
		else
			cmd = buffer.substr(0, delim.second);
		cut = this->trim_buffer(cmd);
		std::cout << "CUT: " << cut << std::endl;
		if ((opt.options & O_SPACE_MODE) && delim.first != '}' && cut.empty())
			return (-1);

		// DELIM is end of block
		if (delim.first == '}') {
			if (opt.blocks.size() == 0 && HTTPConfig::error("Extra '}'", opt.line, opt.options)) { return (1); }
			//else if (cut != "" && HTTPConfig::error("Missing separator", opt.line, opt.options)) { return (1); }
			else {
				if (opt.blocks.top() == "server") {
					opt.current_serv = &(this->default_config);
				}
				opt.blocks.pop();
				if (opt.blocks.empty())
					opt.in_http = false;
			}
		}

		// DELIM is start of block
		else if (delim.first == '{') {
			ret = this->set_block(cut, opt);
			if (ret == 2) {
				this->skip_block(buffer, delim.second);
				continue ;
			}
			opt.blocks.push(cut);
		}

		// DELIM is ; or \n (SPACE_MODE)
		else if (!cut.empty()) { ret = understand_the_cut(cut, opt); }
		buffer = buffer.substr(delim.second + 1 - (delim.first == '\n'));
		if (ret == 1)
			return (ret);
	}
	return (ret);
}


int	HTTPConfig::understand_the_cut(std::string & cut, HTTPConfig::t_parser &opt) {
	if (cut[0] == '#')
		return (0);
	if (cut.substr(0, 6) == "DEFINE") {
		return (this->set_define(cut, opt));
	}
	else if (opt.blocks.size() != 0 && opt.blocks.top() == "types")
		return (this->set_type(cut, opt));
	else {
		if (!opt.in_http && HTTPConfig::warning("Not DEFINE not in a HTTP block", opt.line, opt.options)) { return (1); }
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
			HTTPConfig::error("No URI for location (if root, specify '/')", opt.line, opt.options);
			return (2 - (opt.options & O_ERROR_STOP));
		}
		else if (split.size() > 2 && HTTPConfig::warning("Multiple URI for location (not supported)", opt.line, opt.options)) { return (1); }
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

	// SERVER
	else if (method == "server") {
		t_config	tmp;
		tmp = this->default_config;
		tmp.port = 80;
		if (split.size() >= 2) {
			tmp.port = std::atoi(split[1].c_str());
			if (tmp.port == 0 && warning("Invalid server port at declaration", opt.line, opt.options)) { return (1); }
		}
		if (split.size() > 2 && warning("Too many ports at server declaration", opt.line, opt.options)) { return (1); }
		this->servers.push_back(tmp);
		opt.current_serv = &(this->servers.back());
	}

	else if (method != "types") {
		return (2 - error("Unknown block type", opt.line, opt.options));
	}
	return (0);
}


int	HTTPConfig::set_define(std::string & cut, HTTPConfig::t_parser &opt) {
	if (opt.blocks.size() != 0 && HTTPConfig::error("Invalid define location", opt.line, opt.options)) { return (1); }

	int	start = 6;
	for (; isspace(cut[start]); start++) {}
	std::string	method = cut.substr(start);

	if (method == "SPACE_MODE")
		return (HTTPConfig::warning("Misplaced define", opt.line, opt.options));
	else if (method == "ERROR_STOP")
		opt.options |= O_ERROR_STOP;
	else if (method == "WARNING_AS_ERROR")
		opt.options |= O_WARNING_AS_ERROR;
	else if (method == "SILENT")
		opt.options |= O_SILENT;
	else if (method == "TOGGLE_BOOL")
		opt.options |= O_TOGGLE_BOOL;
	else {
		return (HTTPConfig::error("Unknown define", opt.line, opt.options));
	}
	return (0);
}

int	HTTPConfig::set_type(std::string & cut, HTTPConfig::t_parser &opt) {
	std::vector<std::string>	split;
	split_cut(split, cut);

	if (split.size() != 2) {
		return (this->error("Type definition does not have 2 arguments", opt.line, opt.options));
	}

	t_type	*t = &(opt.current_serv->types);
	if (t->find(split[0]) != t->end()) {
		(*t)[split[0]] = split[1];
		return (this->warning("Type already set : " + split[0], opt.line, opt.options));
	}
	t->insert(std::pair<std::string, std::string>(split[0], split[1]));
	return (0);
}


int	HTTPConfig::set_other(std::string & cut, HTTPConfig::t_parser &opt) {
	std::vector<std::string>	split;
	split_cut(split, cut);

	t_config	*serv = opt.current_serv;

	std::string	method = split.front();

	// LOCATION METHODS
	if (this->in(method, "root", "alias", "index", NULL)) {
		if (opt.blocks.top().substr(0, 8) != "location") { return (HTTPConfig::error(method + " outside of 'location' block", opt.line, opt.options)); }
		else {
			HTTPConfig::t_location	*tmp = &(serv->locations.back());
			if (method == "root" || method == "alias") {
				if (tmp->replacement != "" && HTTPConfig::warning("Overwriting already existing alias", opt.line, opt.options)) { return (1); }
				if (split.size() == 1) { return (HTTPConfig::warning("No location for a uri", opt.line, opt.options)); }
				if (split.size() != 2 && HTTPConfig::warning("Multiple locations for a uri", opt.line, opt.options)) { return (1); }
				tmp->replacement = split[1];
				tmp->alias = (method == "alias");
			} else {
				if (split.size() == 1) { return (this->error("Not enough arguments", opt.line, opt.options)); }
				else if (split.size() > 2 && this->warning("Too many arguments", opt.line, opt.options)) { return (1); }
				serv->locations.back().index = split[1];
			}
		}
	}

	// BOOLEAN METHODS
	// For the TOGGLE_BOOL define, we use one of the properties of the NAND gate
	// 1 NAND 1 = 0, 0 NAND 1 = 1 so if B = 1, A is 'switched'
	// 1 NAND 0 = 1, 0 NAND 0 = 1 so if B = 0, A is true
	else if (this->in(method, "absolute_redirect", "chunked_transfer_encoding",
			"ignore_invalid_headers", "log_not_found", "log_subrequest", NULL)) {
		bool	on;
		if (split.size() == 1) { on = (opt.options & O_TOGGLE_BOOL); }
		else {
			if (split.size() > 2 && this->warning("More than one argument to boolean method " + method, opt.line, opt.options)) { return (1); }
			on = (split[1] == "on");
		}

		switch (method[0]) { // ugly switch
			case 'a':
				serv->absolute_redirect = !(serv->absolute_redirect & on);
				break ;
			case 'c':
				serv->chunked_transfer_encoding = !(serv->chunked_transfer_encoding & on);
				break ;
			case 'i':
				serv->ignore_invalid_headers = !(serv->ignore_invalid_headers & on);
				break ;
			default: // l
				if (method == "log_not_found")
					serv->log_not_found = !(serv->log_not_found & on);
				else
					serv->log_subrequest = !(serv->log_subrequest & on);
				break ;
		}
	}

	// ARGUMENTAL METHODS
	else {
		if (split.size() == 1) { return (this->error("Not enough arguments", opt.line, opt.options)); }
		else if (!this->in(method, "error_page", "add_header", NULL) && split.size() > 2 && this->warning("Too many arguments", opt.line, opt.options)) { return (1); }

		long	ret = std::atol(split[1].c_str());
		switch (method[0]) {
			case 'c':
				if (method == "client_body_timeout") {
					ret = this->translate_time(split[1]);
					if (ret == -1) { return this->error("Invalid argument", opt.line, opt.options); }
					serv->client_body_timeout = ret;
				}
				else if (method == "client_body_buffer_size") { serv->client_body_buffer_size = ret;}
				else if (method == "client_body_in_file_only") {
					if (split[1] == "clean") { ret = -1; }
					else { ret = (split[1] == "on"); }
					serv->client_body_in_file_only = ret;
				}
				else if (method == "client_header_buffer_size") { serv->client_header_buffer_size = ret; }
				else if (method == "client_max_body_size") { serv->client_max_body_size = ret; }
				else { return (this->unknown_command_error(opt)); }
				break ;

			case 'd':
				if (method != "default_type") { return (this->unknown_command_error(opt)); }
				serv->default_type = split[1];
				break ;

			case 'k':
				if (method == "keepalive_requests") { serv->keepalive_requests = ret; }
				else if (method == "keepalive_time") {
					ret = this->translate_time(split[1]);
					if (ret == -1) { return this->error("Invalid argument", opt.line, opt.options); }
					serv->keepalive_time = ret;
				}
				else
					return (this->unknown_command_error(opt));
				break ;


			default:
				if (method == "listen") {
					if (ret > std::numeric_limits<int>::max() || ret < 0) {
						return (this->error("Invalid port", opt.line, opt.options));
					}
					serv->port = ret;
				}
				else if (method == "server_name")
					serv->server_name = split[1];
				else if (method == "error_page")
					return (this->set_error_page(split, opt));
				else if (method == "add_header") {
					if (split.size() < 3) { return (0); }
					serv->headers.insert(std::pair<std::string, std::string>(split[1], split[2]));
				}
				else
					return (this->unknown_command_error(opt));
		}
	}
	return (0);
}

int	HTTPConfig::set_error_page(std::vector<std::string> &split, t_parser &opt) {
	unsigned int	index = 1;
	HTTPConfig::t_config	*serv = opt.current_serv;
	HTTPConfig::t_error		err;

	std::vector<int>	codes;
	int	error_code;
	for (; index < split.size() && split[index][0] != '='; index++) {
		error_code = std::atoi(split[index].c_str());
		if ((error_code < 100 || error_code >= 600) && this->warning("Invalid error code", opt.line, opt.options)) { return (1); }
		codes.push_back(error_code);
	}

	err.codes = codes;
	if (split[index][0] == '=' && index != split.size()) {
		err.response = std::atoi(split[index].c_str() + 1);
		index++;
	}
	err.uri = split[index];
	serv->error_page.push_back(err);
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
	buffer.substr(start + 1);
}

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


bool	HTTPConfig::warning(std::string const message, unsigned long line, int mask) {
	if (!(mask & O_SILENT)) {
		std::cerr << "[WARNING] " << message;
		if (line != 0) {
			std::cerr << " [l." << line << "]";
		}
		std::cerr << std::endl;
	}
	return ((mask & O_WARNING_AS_ERROR) && (mask & O_ERROR_STOP));
}

bool	HTTPConfig::error(std::string const message, unsigned long line, int mask) {
	if (!(mask & O_SILENT)) {
		std::cerr << "[ERROR] " << message;
		if (line != 0) {
			std::cerr << " [l." << line << "]";
		}
		std::cerr << std::endl;
	}
	return (mask & O_ERROR_STOP);
}
