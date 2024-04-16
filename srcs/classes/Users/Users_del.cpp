#include "Users.hpp"

void	Users::handle_del(t_response_creator &rc) {
	for (std::vector<t_session>::iterator it = this->sessions.begin();
			it != this->sessions.end(); it++) {
		if (it->auth == rc.req.body) {
			this->sessions.erase(it);
			break ;
		}
	}
	rc.res.headers += "Set-Cookie: session_id=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; Secure; SameSite=Strict" + std::string(CRLF); // Resets value to empty and make it expired. According to the RFC, only an expired date is necesary, but you just never know...
	return ;
}
