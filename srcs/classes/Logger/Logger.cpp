#include "Logger.hpp"

void	Logger::log_it(HTTPServ::t_socket *socket) const {
	std::vector<HTTPConfig::t_log> *l;
	if (socket->rc.location == NULL) {
		 l = &(socket->rc.conf->default_root.logs);
	} else {
		l = &(socket->rc.location->logs);
	}

	bool					def_log = false, foc_log = false;
	std::set<std::string>	ind_log;
	for (std::vector<HTTPConfig::t_log>::const_iterator it = l->begin(); it != l->end(); it++) {
		if ((it->tag == FOCUSED_LOG && foc_log) ||
				(it->tag == INDEPENDANT_LOG && ind_log.find(it->file_name) != ind_log.end()) ||
				(it->tag != FOCUSED_LOG && it->tag != INDEPENDANT_LOG && def_log)) {
			continue ;
		}
		else if (this->log_match(*it, socket->rc.err_code)) {
			if (it->tag == FOCUSED_LOG) { foc_log = true; }
			else if (it->tag == INDEPENDANT_LOG) { ind_log.insert(it->file_name); }
			else { def_log = true; }
			this->log_in_file(*it, socket);
		}
	}
}

void	Logger::log_in_file(HTTPConfig::t_log const &l, HTTPServ::t_socket const *socket) const {
	std::ofstream	log_file;

	this->open_log_file(l, log_file);
	if (!log_file.good()) { return ; }
	t_response_creator const &rc = socket->rc;
	std::string	host;
	std::map<std::string, std::vector<std::string> >::const_iterator f = rc.req.headers.find("host");
	if (f != rc.req.headers.end()) {
		for (std::vector<std::string>::const_iterator it = f->second.begin();
				it != f->second.end(); it++) {
			host = *it;
		}
	} else {
		host = "-";
	}
	this->print_std_log(log_file, rc.ip, rc.req.method + " " + rc.req.uri + " " + rc.req.http_version, rc.err_code, rc.res.status_line.size() + rc.res.headers.size() + rc.res.body.size(), host);
}

void	Logger::print_std_log(std::ofstream &out, const char *ip, std::string const &req, short err_code, unsigned long res_length, std::string const &client) const {
	out << ip << " - - ";
	this->print_formated_date(out);
	out << " \"" << req << "\" " << err_code << " ";
	if (res_length == 0) {
		out << "-";
	} else {
		out << res_length;
	}
	out << " - " << client << std::endl;
	return ;
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

void	Logger::open_log_file(HTTPConfig::t_log const &l, std::ofstream &out) {
	switch (l.tag) {
		case FOCUSED_LOG:
			out.open("./miniweb.focused.log", std::ios_base::app);
			break ;
		case INDEPENDANT_LOG:
			out.open(l.file_name.c_str(), std::ios_base::app);
			break ;
		default:
			out.open("./miniweb.log", std::ios_base::app);
	}
}

bool	Logger::cmp_err_text(std::string const & err_code, std::string const & text) {
	std::cout << err_code << " | " << text << std::endl;
	if (text == "all") { return (true); }
	else if (text == "errors" && (err_code[0] == '4' || err_code[0] == '5')) { return (true); }
	else if (text == "server" && (err_code[0] == '5')) { return (true); }
	else if (text == "client" && (err_code[0] == '4')) { return (true); }
	return (false);
}
