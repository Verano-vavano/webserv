#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_get(t_response &res, t_request &req) {
	if (req.uri.empty()) {
		(void) res;
		return ;
	}


}
