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
		if (opt.blocks.size() != 0 && opt.blocks.top() != "server" && opt.blocks.top() != "http") {
			return (2 - HTTPConfig::error("Invalid location for 'location' block", opt.line, opt.options));
		}
		if (split.size() == 1) {
			HTTPConfig::error("No URI for location (if root, specify '/')", opt.line, opt.options);
			return (2 - (opt.options & O_ERROR_STOP));
		}
		else if (split.size() > 2 && HTTPConfig::warning("Multiple URI for location (not supported)", opt.line, opt.options)) { return (1); }
		std::string	default_uri = HTTPProtocol::remove_useless_slashes(split[1]);
		if (default_uri.size() && default_uri[default_uri.size() - 1] != '/') { default_uri += "/"; }
		t_location	*tmp = this->get_location(*opt.current_serv, default_uri);
		opt.blocks.push("location");
		if (tmp != NULL) {
			opt.current_location = tmp;
			return (0);
		}
		t_location	tmp2 = opt.current_serv->default_root;
		tmp2.default_uri = default_uri;
		opt.current_serv->locations.push_back(tmp2);
		opt.current_location = &(opt.current_serv->locations.back());
		return (0);
	}

	// HTTP
	else if (method == "http") {
		if (!opt.blocks.empty()) {
			std::cerr << "[FATAL ERROR] http block not in global scope at line " << opt.line << std::endl;
			return (1);
		}
		opt.blocks.push("http");
		opt.in_http = true;
	}

	// SERVER
	else if (method == "server") {
		std::pair<std::string, int>	server_name;
		if (split.size() >= 2) {
			if (split_server_name(split[1], server_name)) { return (1); }
		}
		if (split.size() > 2 && warning("Too many server_name.seconds at server declaration", opt.line, opt.options)) { return (1); }
		t_config	*tmp = this->get_config(server_name.second);
		opt.blocks.push("server");
		if (tmp) {
			opt.current_serv = tmp;
			return (0);
		}
		t_config	tmp2;
		tmp2 = this->default_config;
		tmp2.port = server_name.second;
		this->servers.push_back(tmp2);
		opt.current_serv = &(this->servers.back());
		opt.current_location = &(opt.current_serv->default_root);
	}

	else if (method != "types") {
		return (2 - error("Unknown block type", opt.line, opt.options));
	} else
		opt.blocks.push("types");
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

void	HTTPConfig::set_methods_rescue_funk(std::string const &entry, t_location *location, bool allow) {
	if (allow) {
		location->methods.insert(entry);
	} else {
		location->methods.erase(entry);
	}
}

int	HTTPConfig::set_methods(std::vector<std::string> const & split, t_parser &opt) {
	bool	allow = (split[0] == "methods");
	t_location	*location = this->get_cur_location(opt);

	bool		notd;
	std::string	entry;
	for (std::vector<std::string>::const_iterator it = split.begin() + 1; it !=split.end(); it++) {
		notd = (it->at(0) == '/');
		entry = this->to_upper(it->substr(it->find_first_not_of("/")));
		if (entry == "STD") {
			entry = "GET";
			this->set_methods_rescue_funk(entry, location, (allow == !notd));
			entry = "POST";
			this->set_methods_rescue_funk(entry, location, (allow == !notd));
			entry = "DELETE";
		}
		this->set_methods_rescue_funk(entry, location, (allow == !notd));
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
	} else if (method == "log") { // LOG
		return (this->set_logs(split, opt));
	} else if (method == "unlog") { // UNLOG
		return (this->unset_logs(split, opt));
	}

	// LOCATION METHODS
	else if (this->in(method, "root", "alias", "index", "cgi_exec", "cgi_interpreter", "dir_listing", "post_func", "del_func", "func", NULL)) {
		HTTPConfig::t_location	*tmp = this->get_cur_location(opt);
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
		} else {
			// FUNC METHODS
			if (split.size() >= 2) {
				std::string	func = this->to_upper(split[1]);
				if (method == "func" || method == "post_func")
					tmp->post_func = func;
				if (method == "func" || method == "del_func")
					tmp->del_func = func;
			}
			else { return (this->error("Need one argument", opt.line, opt.options)); }
		}
	}

	// BOOLEAN METHODS
	else if (this->in(method, "absolute_redirect", "chunked_transfer_encoding",
			"ignore_invalid_headers", "default_interpreter", NULL)) {
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
		}
	}

	// ARGUMENTAL METHODS
	else {
		if (split.size() == 1) { return (this->error("Not enough arguments for an argumental method (aka default)", opt.line, opt.options)); }
		else if (!this->in(method, "error_page", "add_header", NULL) && split.size() > 2 && this->warning("Too many arguments", opt.line, opt.options)) { return (1); }

		long	ret = std::max((long) 2, std::atol(split[1].c_str()));
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

int	HTTPConfig::set_error_page(std::vector<std::string> &split, t_parser &opt) const {
	unsigned int	index = 1;
	HTTPConfig::t_config	*serv = opt.current_serv;
	HTTPConfig::t_error		err;

	std::set<int>	codes;
	int				error_code;
	for (; index < split.size() - 1 && isallnum(split[index]); index++) {
		error_code = std::atoi(split[index].c_str());
		if ((error_code < 100 || error_code >= 600) && this->warning("Invalid error code", opt.line, opt.options)) { return (1); }
		codes.insert(error_code);
	}

	err.codes = codes;
	err.response = -1;

	bool	uri_set = false;
	bool	equal_set = false;
	for (; index < split.size(); index++) {
		if (!equal_set && split[index][0] == '=') {
			err.response = std::atoi(split[index].c_str() + 1);
			equal_set = true;
			if (uri_set) { break ; }
		} else if (!uri_set && split[index][0] != '=') {
			if (split[index][0] != '/')
				err.uri = "/" + split[index];
			else
				err.uri = split[index];
			uri_set = true;
			if (equal_set) { break ; }
		}
	}
	serv->error_page.push_back(err);
	return (0);
}
