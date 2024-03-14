#include "HTTPProtocol.hpp"

void	HTTPProtocol::set_headers(t_response_creator &r) {
	std::string	&h = r.res.headers;
	std::ostringstream	s;

	h += "Content-Type: " + r.file_type + CRLF;
	s << r.res.body.size();
	h += "Content-Length: " + s.str() + CRLF;
}
