#include "HTTPConfig.hpp"

// Debug purposes
void	HTTPConfig::print_config(void) const {
	this->print_server(default_config);
	for (std::vector<t_config>::const_iterator it = this->servers.begin();
			it != this->servers.end();
			it++) {
		this->print_server(*it);
	}
	return ;
}

// For debug purposes
void	HTTPConfig::print_server(t_config const &s) {
	std::cout << "--SERVER--" << "\n"
		<< "port = " << s.port << "\n"
		<< "server_name = " << s.server_name << "\n"
		<< "default_root :\n" << s.default_root << "\n"
		<< "absolute_redirect = " << s.absolute_redirect << "\n"
		<< "chunked_transfer_encoding = " << s.chunked_transfer_encoding << "\n"
		<< "client_body_timeout = " << s.client_body_timeout << "\n"
		<< "client_body_buffer_size = " << s.client_body_buffer_size << "\n"
		<< "client_body_in_file_only = " << s.client_body_in_file_only << "\n"
		<< "client_header_buffer_size = " << s.client_header_buffer_size << "\n"
		<< "client_max_body_size = " << s.client_max_body_size << "\n"
		<< "default_type = " << s.default_type << "\n"
		<< "ignore_invalid_headers = " << s.ignore_invalid_headers << "\n"
		<< "keepalive_requests = " << s.keepalive_requests << "\n"
		<< "keepalive_time = " << s.keepalive_time << "\n"
		<< "log_not_found = " << s.log_not_found << "\n"
		<< "log_subrequest = " << s.log_subrequest << "\n"
		<< "default_interpreter = " << s.default_interpreter << std::endl;
	std::cout << "Error pages :" << std::endl;
	for (std::vector<HTTPConfig::t_error>::const_iterator it = s.error_page.begin();
			it != s.error_page.end(); it++) {
		std::cout << "- ";
		for (std::set<int>::const_iterator it2 = it->codes.begin(); it2 != it->codes.end(); it2++) {
			std::cout << *it2 << " ";
		}
		std::cout << "=> " << it->uri << " (" << it->response << ")" << std::endl;
	}
	std::cout << "Locations :" << std::endl;
	for (std::vector<HTTPConfig::t_location>::const_iterator it = s.locations.begin();
			it != s.locations.end(); it++) {
		std::cout << *it << "\n";
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

std::ostream&	operator<<(std::ostream& out, HTTPConfig::t_location const & obj) {
	out << "\tDEF = " << obj.default_uri << "\n";
	out << "\tREPL = " << obj.replacement << "\n";
	out << "\tIND = " << obj.index << "\n";
	out << "\tALIAS = " << obj.alias << "\n";
	out << "\tDIR = " << obj.dir_listing << "\n";
	out << "\tMETHODS =";
	for (std::set<std::string>::const_iterator it = obj.methods.begin(); it != obj.methods.end(); it++) {
		out << " " << *it;
	}
	return (out);
}


// Warn and bitmask check
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

// Err and bitmask check
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
