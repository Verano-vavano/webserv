#include "HTTPConfig.hpp"

void HTTPConfig::configurate(std::string const path, std::string const config_file) {
	if (config_file.find(".conf") == std::string::npos) {
		std::string choice;
		std::cout << "Not a .conf file. Do you wish to continue ? (Y/N) > ";
		std::getline(std::cin, choice);
		if (choice != "Y" && choice != "y")
			exit (0);
		else
			std::cout << "Proceeding..." << std::endl;
	}

	this->path = path.substr(0, path.find_last_of("/") + 1);
	this->set_default_config();

	// We check if the first line is 'DEFINE SPACE_MODE;'
	// It enables space mode, so it uses getline instead of read
	std::string	first_line;
	{
		std::ifstream	config(config_file.c_str());
		if (!config || !config.good()) {
			std::cerr << "Not a valid config file (does not exist or is not readable)" << std::endl;
			exit (EXIT_FAILURE);
		}

		std::getline(config, first_line);
		first_line = this->trim_buffer(first_line);
	}

	bool	space_mode = (first_line.size() >= 18 && first_line.substr(0, 18) == "DEFINE SPACE_MODE;");

	// We open the file again to move the cursor at the start
	std::ifstream	new_config(config_file.c_str());
	if (!new_config || !new_config.good()) {
		std::cerr << "Not a valid config file (does not exist or is not readable)" << std::endl;
		exit(EXIT_FAILURE) ;
	}

	this->parse_infile(new_config, space_mode);
	if (this->servers.empty()) {
		this->servers.push_back(this->default_config);
	}
}

void	HTTPConfig::set_default_config(void) {
	t_config	&conf = this->default_config;

	conf.port = DEFAULT_PORT;
	conf.server_name = DEFAULT_NAME;
	conf.default_root.default_uri = "/";
	conf.default_root.replacement = DEFAULT_LOCATION;
	conf.default_root.dir_listing = false;
	conf.default_root.post_func = "NONE";
	conf.default_root.alias = false;
	conf.default_root.methods.insert("GET");
	conf.default_root.methods.insert("POST");
	conf.default_root.methods.insert("DELETE");
	t_log	default_log;
	default_log.tag = DEFAULT_LOG;
	default_log.err_codes.push_back("errors");
	conf.default_root.logs.push_back(default_log);
	conf.absolute_redirect = DEFAULT_REDIR;
	conf.chunked_transfer_encoding = DEFAULT_CHUNKED;
	conf.client_body_timeout = DEFAULT_BODY_TO;
	conf.client_body_buffer_size = DEFAULT_BODY_BUF_SIZE;
	conf.client_body_in_file_only = DEFAULT_BODY_IN;
	conf.client_header_buffer_size = DEFAULT_HEAD_BUF_SIZE;
	conf.client_max_body_size = DEFAULT_MAX_BODY;
	conf.default_type = DEFAULT_TYPE;
	conf.ignore_invalid_headers = DEFAULT_IGNORE_INVALID;
	conf.keepalive_requests = DEFAULT_KEEPALIVE_REQ;
	conf.keepalive_time = DEFAULT_KEEPALIVE_TIME;
	conf.default_interpreter = DEFAULT_INTERPRETER;
	conf.path = this->path;

	return ;
}

HTTPConfig::t_config	& HTTPConfig::t_config::operator=(t_config const & rhs) {
	if (this == &rhs)
		return (*this);


	this->port = rhs.port;
	this->server_name = rhs.server_name;
	this->default_root = rhs.default_root;
	this->absolute_redirect = rhs.absolute_redirect;
	this->chunked_transfer_encoding = rhs.chunked_transfer_encoding;
	this->client_body_timeout = rhs.client_body_timeout;
	this->client_body_buffer_size = rhs.client_body_buffer_size;
	this->client_body_in_file_only = rhs.client_body_in_file_only;
	this->client_header_buffer_size = rhs.client_header_buffer_size;
	this->client_max_body_size = rhs.client_max_body_size;
	this->default_type = rhs.default_type;
	this->ignore_invalid_headers = rhs.ignore_invalid_headers;
	this->keepalive_requests = rhs.keepalive_requests;
	this->keepalive_time = rhs.keepalive_time;
	this->default_interpreter = rhs.default_interpreter;
	this->path = rhs.path;

	HTTPConfig::t_error	tmp;
	for (std::vector<HTTPConfig::t_error>::const_iterator it = rhs.error_page.begin();
			it != rhs.error_page.end(); it++) {
		tmp = *it;
		this->error_page.push_back(tmp);
	}

	HTTPConfig::t_location	tmp2;
	for (std::vector<HTTPConfig::t_location>::const_iterator it = rhs.locations.begin();
			it != rhs.locations.end(); it++) {
		tmp2 = *it;
		this->locations.push_back(tmp2);
	}

	copy_map_strstr(this->types, rhs.types);
	copy_map_strstr(this->headers, rhs.headers);

	return (*this);
}

HTTPConfig::t_error	& HTTPConfig::t_error::operator=(t_error const & rhs) {
	if (this == &rhs)
		return (*this);

	this->response = rhs.response;
	this->uri = rhs.uri;
	std::set<int>	tmp(rhs.codes.begin(), rhs.codes.end());
	this->codes = tmp;
	return (*this);
}

HTTPConfig::t_location & HTTPConfig::t_location::operator=(t_location const & rhs) {
	if (this == &rhs)
		return (*this);

	this->default_uri = rhs.default_uri;
	this->replacement = rhs.replacement;
	this->index = rhs.index;
	this->post_func = rhs.post_func;
	this->alias = rhs.alias;
	this->dir_listing = rhs.dir_listing;

	for (std::set<std::string>::const_iterator it = rhs.methods.begin(); it != rhs.methods.end(); it++) {
		this->methods.insert(*it);
	}

	std::string	tmp;
	for (std::set<std::string>::const_iterator it = rhs.cgi.cgi_exec.begin(); it != rhs.cgi.cgi_exec.end(); it++) {
		tmp = *it;
		this->cgi.cgi_exec.insert(tmp);
	}

	t_log	log;
	for (std::vector<t_log>::const_iterator it = rhs.logs.begin(); it != rhs.logs.end(); it++) {
		log = *it;
		this->logs.push_back(log);
	}

	HTTPConfig::copy_map_strstr(this->cgi.cgi_interpreter, rhs.cgi.cgi_interpreter);
	return (*this);
}

HTTPConfig::t_log & HTTPConfig::t_log::operator=(t_log const & rhs) {
	this->err_codes = rhs.err_codes;
	this->tag = rhs.tag;
	this->file_name = rhs.file_name;
	return (*this);
}

void	HTTPConfig::copy_map_strstr(t_map_str_str & n, t_map_str_str const & old) {
	std::pair<std::string, std::string>	tmp;

	for (t_map_str_str::const_iterator it = old.begin(); it != old.end(); it++) {
		tmp.first = it->first;
		tmp.second = it->second;
		n.insert(tmp);
	}
}
