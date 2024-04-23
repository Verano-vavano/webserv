#ifndef LOGGER_HPP
# define LOGGER_HPP

# define LOG_FILE_NAME "./miniweb.log"
# define LOG_FOCUSED_FILE_NAME "./miniweb.focused.log"

# include <iostream>
# include <fstream>
# include <ctime>
# include <stdlib.h>
# include "HTTPServ.hpp"

class Logger {

	public:
		Logger(void) {}
		~Logger(void) {}

		void		log_it(HTTPServ::t_socket *socket);
		void		log_fatal(const char *err);
		inline void	log_stderr(const char *err, const char *type) const;

		static bool	cmp_err_text(std::string const & err_code, std::string const & text);

	private:
		static bool	log_match(HTTPConfig::t_log const &l, int err_code_int);
		static void	print_formated_date(std::ofstream &out);
};

#endif /* LOGGER_HPP */
