#ifndef LOGGER_HPP
# define LOGGER_HPP

# define LOG_FILE_NAME "./miniweb.log"
# define LOG_FOCUSED_FILE_NAME "./miniweb.focused.log"

# include <iostream>
# include <fstream>
# include <ctime>

class Logger {

	public:
		Logger(void) {}
		~Logger(void) {}

		void		log_fatal(const char *err);
		inline void	log_stderr(const char *err, const char *type) const;

	private:
		static void	print_formated_date(std::ofstream &out);
};

#endif /* LOGGER_HPP */

