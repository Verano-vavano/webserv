#include "HTTPProtocol.hpp"

void	HTTPProtocol::check_type(t_response_creator &r) {
	std::map<std::string, std::vector<std::string> >::iterator	accept = r.req.headers.find("Accept");
	if (accept == r.req.headers.end()) { return ; }

	std::vector<std::string>	values = accept->second;
	std::pair<std::string, float>	typing;
	typing.first = "NONE";
	typing.second = 0;

	std::string			type, q_str;
	float				q;
	unsigned long			coma_sep;
	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++) {
		coma_sep = it->find(";");
		if (coma_sep > it->size()) {
			type = *it;
			q = 1.f;
		} else {
			type = it->substr(0, coma_sep);
			q_str = it->substr(coma_sep + 1);
			if (q_str.size() < 3 || q_str.size() > 10 || q_str.substr(0, 2) != "q=") { q = 1.f; }
			else {
				q_str = q_str.substr(2);
				q = atof(q_str.c_str());
				if (q < 0) { q = 0; }
				else if (q > 1) {q = 1; }
			}
		}
		if (r.file_type == type || is_wildcard_match(r.file_type, type)) {
			if (q > typing.second) {
				typing.first = r.file_type;
			}
			if (q == 1) { break ; }
		}
	}
	if (typing.first == "NONE") {
		r.err_code = 406;
	} else {
		r.file_type = typing.first;
	}
	return ;
}
