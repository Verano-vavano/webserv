#include "HTTPConfig.hpp"
#include "HTTPProtocol.hpp"

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
		tmp.replacement = "";
		tmp = opt.current_serv->default_root;
		if (split.size() == 1) {
			HTTPConfig::error("No URI for location (if root, specify '/')", opt.line, opt.options);
			return (2 - (opt.options & O_ERROR_STOP));
		}
		else if (split.size() > 2 && HTTPConfig::warning("Multiple URI for location (not supported)", opt.line, opt.options)) { return (1); }
		tmp.default_uri = HTTPProtocol::remove_useless_slashes(split[1]);
		if (tmp.default_uri.size() && tmp.default_uri[tmp.default_uri.size() - 1] != '/') { tmp.default_uri += "/"; }
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
		tmp.port = 80;
		tmp = this->default_config;
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

	unsigned int	start = 6;
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

int	HTTPConfig::set_methods(std::vector<std::string> const & split, t_parser &opt) {
	bool	allow = (split[0] == "methods");
	t_location	*location;
	if (opt.blocks.size() && opt.blocks.top().substr(0, 8) == "location") {
		location = &(opt.current_serv->locations.back());
	} else {
		location = &(opt.current_serv->default_root);
	}

	bool		notd;
	std::pair<std::string, bool>		entry;
	std::map<std::string, bool>::iterator	finder;
	for (std::vector<std::string>::const_iterator it = split.begin() + 1; it !=split.end(); it++) {
		notd = (it->at(0) == '/');
		entry.first = this->to_upper(it->substr(it->find_first_not_of("/")));
		entry.second = (allow == !notd);
		finder = location->methods.find(entry.first);
		if (finder != location->methods.end()) {
			finder->second = entry.second;
		} else {
			location->methods.insert(entry);
		}
	}
	return (0);
}


int	HTTPConfig::set_other(std::string & cut, HTTPConfig::t_parser &opt) {
	std::vector<std::string>	split;
	split_cut(split, cut);

	t_config	*serv = opt.current_serv;

	std::string	method = split.front();

	// METHODS
	if (method == "methods" || method == "not_methods") {
		return (this->set_methods(split, opt));
	}

	// LOCATION METHODS
	else if (this->in(method, "root", "alias", "index", "cgi_exec", "cgi_interpreter", "dir_listing", NULL)) {
		HTTPConfig::t_location	*tmp = &(serv->locations.back());
		if (opt.blocks.size() == 0 || opt.blocks.top().substr(0, 8) != "location") { tmp = &(serv->default_root); }
		if (method == "root" || method == "alias") {
			if (split.size() == 1) { return (HTTPConfig::warning("No location for a uri", opt.line, opt.options)); }
			if (split.size() != 2 && HTTPConfig::warning("Multiple locations for a uri", opt.line, opt.options)) { return (1); }
			tmp->replacement = split[1];
			tmp->alias = (method == "alias");
		} else if (method == "index" || method == "cgi_exec") {
			if (split.size() == 1) { return (this->error("Not enough arguments", opt.line, opt.options)); }
			else if (split.size() > 2 && this->warning("Too many arguments", opt.line, opt.options)) { return (1); }
			if (method == "index")
				tmp->index = split[1];
			else if (method == "cgi_exec")
				tmp->cgi.cgi_exec.insert(split[1]);
		} else if (method == "cgi_interpreter") {
			if (split.size() != 3) { return (this->error("Invalid number of arguments", opt.line, opt.options)); }
			std::pair<std::string, std::string>	p;
			p.first = split[1];
			p.second = split[2];
			tmp->cgi.cgi_interpreter.insert(p);
		} else if (method == "dir_listing") {
			if (this->boolean_switch(tmp->dir_listing, opt, split)) { return (1); };
		}
	}

	// BOOLEAN METHODS
	else if (this->in(method, "absolute_redirect", "chunked_transfer_encoding",
			"ignore_invalid_headers", "log_not_found", "log_subrequest", "default_interpreter", NULL)) {
		switch (method[0]) { // ugly switch on the first letter, saves some CPU time
			case 'a':
				if (this->boolean_switch(serv->absolute_redirect, opt, split)) { return (1); };
				break ;
			case 'c':
				if (this->boolean_switch(serv->chunked_transfer_encoding, opt, split)) { return (1); };
				break ;
			case 'd':
				if (this->boolean_switch(serv->default_interpreter, opt, split)) { return (1); };
				break ;
			case 'i':
				if (this->boolean_switch(serv->ignore_invalid_headers, opt, split)) { return (1); };
				break ;
			default: // l
				if (method == "log_not_found") {
					if (this->boolean_switch(serv->log_not_found, opt, split)) { return (1); }
				} else {
					if (this->boolean_switch(serv->log_subrequest, opt, split)) { return (1); }
				}
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

	std::set<int>	codes;
	int	error_code;
	for (; index < split.size() - 1 && split[index][0] != '='; index++) {
		error_code = std::atoi(split[index].c_str());
		if ((error_code < 100 || error_code >= 600) && this->warning("Invalid error code", opt.line, opt.options)) { return (1); }
		codes.insert(error_code);
	}

	err.codes = codes;
	if (split[index][0] == '=' && index != split.size()) {
		err.response = std::atoi(split[index].c_str() + 1);
		index++;
	}
	else
		err.response = -1;
	err.uri = split[index];
	serv->error_page.push_back(err);
	return (0);
}
