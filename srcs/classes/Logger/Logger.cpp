#include "Logger.hpp"

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