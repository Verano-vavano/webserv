#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_get(t_response_creator &r) {
	if (r.req.uri.empty()) {
		r.err_code = 400;
		return ;
	}

	//std::string	dir = get_dir_uri(r.req.uri, r.conf, &(r.err_code));
}
