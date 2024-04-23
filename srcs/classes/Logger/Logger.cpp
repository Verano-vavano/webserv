#include "Logger.hpp"

void	Logger::log_it(HTTPServ::t_socket *socket) {
	std::vector<HTTPConfig::t_log> *l;
	if (socket->rc.location == NULL) {
		 l = &(socket->rc.conf->default_root.logs);
	} else {
		l = &(socket->rc.location->logs);
	}

	for (std::vector<HTTPConfig::t_log>::const_iterator it = l->begin(); it != l->end(); it++) {
		if (this->log_match(*it, socket->rc.err_code))
				std::cout << "hola que tal" << std::endl;
				//this->log_in_file(*it, socket);
	}
}

void	Logger::log_fatal(const char *err) {
	this->log_stderr(err, "FATAL");

	std::ofstream	log_file(LOG_FILE_NAME, std::ios_base::app);
	if (!log_file || !log_file.good()) {
		return ;
	}

	log_file << "FATAL ERR - - ";
	this->print_formated_date(log_file);
	log_file << " \"" << err << "\" ";
	log_file << "500 0 - -" << std::endl;
	return ;
}

inline void	Logger::log_stderr(const char *err, const char *type) const {
	std::cerr << "[" << type << "] ERROR : " << err << std::endl;
}


bool	Logger::log_match(HTTPConfig::t_log const &l, int err_code_int) {
	std::stringstream	str_obj;
	str_obj << err_code_int;
	std::string	err_code;
	str_obj >> err_code;
	for (std::vector<std::string>::const_iterator it = l.err_codes.begin(); it != l.err_codes.end(); it++) {
		if (HTTPConfig::cmp_err_code(err_code, *it) || Logger::cmp_err_text(err_code, *it)) {
			std::cout << "FOUND " << err_code << " with " << *it << std::endl;
			return (true);
		}
	}
	return (false);
}

bool	Logger::cmp_err_text(std::string const & err_code, std::string const & text) {
	if (text == "all") { return (true); }
	else if (text == "errors" && (err_code[0] == '4' || err_code[0] == '5')) { return (true); }
	else if (text == "server" && (err_code[0] == '5')) { return (true); }
	else if (text == "client" && (err_code[0] == '4')) { return (true); }
	return (false);
}
