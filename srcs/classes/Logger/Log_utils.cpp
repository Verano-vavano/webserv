#include "Logger.hpp"

short	Logger::is_to_log(HTTPConfig::t_location *l, t_response_creator &rc) {
	for (std::vector<HTTPConfig::t_log>::const_iterator it = l->logs.begin(); it != l->logs.end(); it++) {
		for (std::vector<std::string>::const_iterator sub_it = it->err_codes.begin(); sub_it != it->err_codes.end(); sub_it++) {
			if (*sub_it == "all" || *sub_it)

		}
	}
	return (true);
}

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
