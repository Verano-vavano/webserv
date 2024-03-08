#include "HTTPConfig.hpp"

void	HTTPConfig::print_config(void) const {
	this->print_server(default_config);
	for (std::vector<t_config>::const_iterator it = this->servers.begin();
			it != this->servers.end();
			it++) {
		this->print_server(*it);
	}
	return ;
}

void	HTTPConfig::print_server(t_config const &s) {
	std::cout << "--SERVER--" << std::endl
		<< "port = " << s.port << std::endl
		<< "server_name = " << s.server_name << std::endl
		<< "absolute_redirect = " << s.absolute_redirect << std::endl
		<< "chunked_transfer_encoding = " << s.chunked_transfer_encoding << std::endl
		<< "client_body_timeout = " << s.client_body_timeout << std::endl
		<< "client_body_buffer_size = " << s.client_body_buffer_size << std::endl
		<< "client_body_in_file_only = " << s.client_body_in_file_only << std::endl
		<< "client_header_buffer_size = " << s.client_header_buffer_size << std::endl
		<< "client_max_body_size = " << s.client_max_body_size << std::endl
		<< "default_type = " << s.default_type << std::endl
		<< "ignore_invalid_headers = " << s.ignore_invalid_headers << std::endl
		<< "keepalive_requests = " << s.keepalive_requests << std::endl
		<< "keepalive_time = " << s.keepalive_time << std::endl
		<< "log_not_found = " << s.log_not_found << std::endl
		<< "log_subrequest = " << s.log_subrequest << std::endl;
	std::cout << "Error pages :" << std::endl;
	for (std::vector<HTTPConfig::t_error>::const_iterator it = s.error_page.begin();
			it != s.error_page.end(); it++) {
		std::cout << "- ";
		for (std::vector<int>::const_iterator it2 = it->codes.begin(); it2 != it->codes.end(); it2++) {
			std::cout << *it2 << " ";
		}
		std::cout << "=> " << it->uri << " (" << it->response << ")" << std::endl;
	}
	std::cout << "Locations :" << std::endl;
	for (std::vector<HTTPConfig::t_location>::const_iterator it = s.locations.begin();
			it != s.locations.end(); it++) {
		std::cout << "- " << it->default_uri << " => " << it->replacement << " . " << it->index << " (" << it->alias << ")" << std::endl;
	}
	std::cout << "Types :" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = s.types.begin(); it != s.types.end(); it++) {
		std::cout << "- " << it->first << " | " << it->second << std::endl;
	}
	std::cout << "Headers :" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = s.headers.begin(); it != s.headers.end(); it++) {
		std::cout << "- " << it->first << " | " << it->second << std::endl;
	}
	return ;
}
