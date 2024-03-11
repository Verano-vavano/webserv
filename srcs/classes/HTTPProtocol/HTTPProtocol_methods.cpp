#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_get(t_response &res, t_request &req) {
	if (req.uri.empty()) {
		req.err_code = 500;
		return ;
	}


}
