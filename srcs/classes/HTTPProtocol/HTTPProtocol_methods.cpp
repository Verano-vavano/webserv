#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_method(t_response_creator &r) {
	std::string	better_uri = remove_useless_slashes(r.req.uri);
	std::string	uri_with_slash = better_uri;
	if (uri_with_slash[uri_with_slash.size() - 1] != '/') { uri_with_slash += "/"; }
	HTTPConfig::t_location const	&dir = get_dir_uri(uri_with_slash, r.conf);
	if (r.req.method == "GET") {
		for (std::map<std::string, bool>::const_iterator it = dir.methods.begin(); it != dir.methods.end(); it++) {
			std::cout << "- " << it->first << ":" << it->second << std::endl;
		}
		std::map<std::string, bool>::const_iterator f = dir.methods.find("GET");
		if (f == dir.methods.end() || f->second == true)
			this->handle_get(r);
		else
			r.err_code = 403;
	}
	else if (r.req.method != "POST")
		r.err_code = 501;
}

void	HTTPProtocol::handle_get(t_response_creator &r) {
	std::string	&uri = r.req.uri;
	if (uri.empty() || uri[0] != '/') {
		r.err_code = 400;
		return ;
	}

	this->get_body(uri, r, -1);
}
