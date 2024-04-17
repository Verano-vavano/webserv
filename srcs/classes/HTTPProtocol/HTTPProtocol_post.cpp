#include "HTTPProtocol.hpp"

void	HTTPProtocol::post_client(t_response_creator &r) {
	this->user_manager.handle_user(r);
}
