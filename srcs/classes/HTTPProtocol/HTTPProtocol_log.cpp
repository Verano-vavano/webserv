#include "HTTPProtocol.hpp"

void	HTTPProtocol::log_it(t_response_creator &r) {
	std::ofstream	log_file(LOG_FILE, std::ios_base::app);

	log_file << r.ip << " - - [";

	time_t	now = time(0);
	tm		*ltm = localtime(&now);
	if (ltm) {
		const char *fmt = "%d/%b/%Y:%T %z";
		char		out[200];
		if (std::strftime(out, sizeof(out), fmt, ltm) != 0) {
			log_file << out;
		}
	}
	log_file << "] \"" << r.req.method << " " << r.req.uri << " " << r.req.http_version << "\" "
		<< r.err_code << " "
		<< r.res.status_line.size() + r.res.headers.size() + r.res.body.size() << " -";

	std::map<std::string, std::vector<std::string> >::const_iterator f = r.req.headers.find("host");
	if (f != r.req.headers.end()) {
		for (std::vector<std::string>::const_iterator it = f->second.begin();
				it != f->second.end(); it++) {
			log_file << " " << *it;
		}
	} else {
		log_file << " -";
	}
	log_file << "\n";
}
