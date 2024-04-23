#include "Logger.hpp"

void	Logger::print_formated_date(std::ofstream &outstream) {
	time_t	now = time(0);
	tm		*ltm = localtime(&now);
	if (ltm) {
		const char *fmt = "[%d/%b/%Y:%T %z]";
		char		out[200];
		if (std::strftime(out, sizeof(out), fmt, ltm) != 0) {
			outstream << out;
		}
	}
}
