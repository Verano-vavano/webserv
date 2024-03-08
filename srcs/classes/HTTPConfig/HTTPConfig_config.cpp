#include "HTTPConfig.hpp"

int HTTPConfig::configurate(std::string const path, std::string const config_file) {
    if (config_file.find(".conf") == std::string::npos) {
        std::string choice;
        std::cout << "Not a .conf file. Do you wish to continue ? (Y/N) > ";
        std::getline(std::cin, choice);
		// TODO return(1) mais plus tard faudra checker dans le constructeur la val de retour non?
        if (choice != "Y" && choice != "y")
            return (1);
        else
            std::cout << "Proceeding..." << std::endl;
    }

	this->set_default_config();

	// TODO on en fait quoi de ca?
    this->path = path;

    std::ifstream    config(config_file.c_str());
    if (!config || !config.good()) {
        std::cerr << "Not a valid config file (does not exist or is not readble)" << std::endl;
        return (1);
    }

    return (this->parse_infile(config));
}

void	HTTPConfig::set_default_config(void) {
	t_config	&conf = this->default_config;

	conf.port = DEFAULT_PORT;
	conf.server_name = DEFAULT_NAME;
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
	conf.log_not_found = DEFAULT_LOG_NOT_FOUND;
	conf.log_subrequest = DEFAULT_LOG_SUB;

	return ;
}

HTTPConfig::t_config	& HTTPConfig::t_config::operator=(t_config const & rhs) {
	if (this == &rhs)
		return (*this);


	this->port = rhs.port;
	this->server_name = rhs.server_name;
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
	this->log_not_found = rhs.log_not_found;
	this->log_subrequest = rhs.log_subrequest;

	HTTPConfig::t_error	tmp;
	for (std::vector<HTTPConfig::t_error>::const_iterator it = rhs.error_page.begin();
			it != rhs.error_page.end(); it++) {
		tmp = *it;
		this->error_page.push_back(tmp);
	}

	// TYPES
	// HEADERS
	// LOCATIONS

	return (*this);
}

HTTPConfig::t_error	& HTTPConfig::t_error::operator=(t_error const & rhs) {
	if (this == &rhs)
		return (*this);

	this->response = rhs.response;
	this->uri = rhs.uri;
	std::vector<int>	tmp(rhs.codes.begin(), rhs.codes.end());
	this->codes = tmp;
	return (*this);
}
