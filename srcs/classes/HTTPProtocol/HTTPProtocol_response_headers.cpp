#include "HTTPProtocol.hpp"

void	set_headers(t_response_creator &r) {
	std::string	&h = r.res.headers;
	(void) h;

	//h += "Content-Type: " + get_mime_type(r.file_type) + CRLF;
}
