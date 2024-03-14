#include "HTTPProtocol.hpp"

void	HTTPProtocol::check_type(t_response_creator &r) {
	std::map<std::string, std::vector<std::string> >::iterator	accept = r.req.headers.find("Accept");
	if (accept == r.req.headers.end()) { return ; }

	std::vector<std::string>	values = accept->second;
	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++) {
		if (r.file_type == *it || it->substr(0, 1) == "*" || it->substr(0, 3) == "*/*") { return ; }
	}
	r.err_code = 406;
	return ;
}
