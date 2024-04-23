#include "HTTPConfig.hpp"

bool	HTTPConfig::is_and_set_tag(std::string const & str, t_log &log) {
	if (str == "default") {
		log.tag = DEFAULT_LOG;
	} else if (str == "focused") {
		log.tag = FOCUSED_LOG;
	} else if (str == "independant") {
		log.tag = INDEPENDANT_LOG;
	} else {
		return (false);
	}
	return (true);
}

bool	HTTPConfig::is_valid_log(std::string const & str) {
	if (str == "all" || str == "fatal" || str == "errors" || str == "client" || str == "server" || str == "nothing") { return (true); }
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
		if (!std::isdigit(*it) && *it != 'x') { return (false); }
	}
	return (true);
}


void	HTTPConfig::unlog_one(t_location *loc, std::string const & to_rm, short tag) {
	std::vector<t_log>::iterator it = loc->logs.begin();
	while (it != loc->logs.end()) {
		if (to_rm == "all" && (tag == UNDEFINED_LOG || tag == it->tag)) {
			loc->logs.erase(it);
		} else {
			it++;
		}
	}
}

void	HTTPConfig::unlog(t_location *loc, std::string const & to_rm, short tag) const {
	if (to_rm == "errors") {
		unlog(loc, "client", tag);
		unlog(loc, "server", tag);
	} else if (to_rm == "client") {
		unlog_one(loc, "4xx", tag);
	} else if (to_rm == "server") {
		unlog_one(loc, "5xx", tag);
		unlog_one(loc, "fatal", tag);
	} else {
		unlog_one(loc, to_rm, tag);
	}
}


int	HTTPConfig::set_logs(std::vector<std::string> const & split, t_parser &opt) {
	t_log		new_log;
	std::string	copier;

	new_log.tag = UNDEFINED_LOG;
	for (std::vector<std::string>::const_iterator it = split.begin() + 1; it != split.end(); it++) {
		if (is_and_set_tag(*it, new_log)) { continue ; }
		else if (*it == "nothing") {
			this->unlog(opt.current_location, "all", new_log.tag);
			return (0);
		} else if (is_valid_log(*it)) {
			copier = *it;
			new_log.err_codes.push_back(copier);
		} else {
			return (this->error("Invalid log", opt.line, opt.options));
		}
	}
	if (new_log.err_codes.size() == 0) {
		new_log.err_codes.push_back("all");
	}
	if (new_log.tag == UNDEFINED_LOG) {
		new_log.tag = DEFAULT_LOG;
	}
	opt.current_location->logs.push_back(new_log);
	return (0);
}


int HTTPConfig::unset_logs(std::vector<std::string> const & split, t_parser &opt) {
	t_log	tmp;

	tmp.tag = UNDEFINED_LOG;
	for (std::vector<std::string>::const_iterator it = split.begin() + 1; it != split.end(); it++) {
		if (is_and_set_tag(*it, tmp)) { continue ; }
		else if (is_valid_log(*it)) {
			this->unlog(opt.current_location, *it, tmp.tag);
		} else {
			return (this->error("Invalid log", opt.line, opt.options));
		}
	}
	return (0);
}
