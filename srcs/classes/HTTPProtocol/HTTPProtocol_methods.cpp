#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_get(t_response_creator &r) {
	std::string	&uri = r.req.uri;
	if (uri.empty() || uri[0] != '/') {
		r.err_code = 400;
		return ;
	}

	std::string	file = get_complete_uri(uri, r.conf);

	r.file_type = file.substr(file.find_last_of(".") + 1);

	std::ifstream	fs(file.c_str());
	if (!fs || !fs.good()) {
		r.err_code = 404;
		return ;
	}
	this->read_entire_file(r.res.body, fs);
}
