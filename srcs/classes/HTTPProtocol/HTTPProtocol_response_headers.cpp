#include "HTTPProtocol.hpp"

void	HTTPProtocol::set_headers(t_response_creator &r) {
	std::string	&h = r.res.headers;
	std::ostringstream	s;

	h += "Content-Type: " + r.file_type + CRLF;
	if (!r.conf->chunked_transfer_encoding) {
		s << r.res.body.size();
		h += "Content-Length: " + s.str() + CRLF;
	} else {
		h += "Transfer-Encoding: chunked" + std::string(CRLF);
	}
}
