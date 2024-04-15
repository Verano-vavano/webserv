#include "HTTPProtocol.hpp"

void	HTTPProtocol::set_headers(t_response_creator &r) {
	std::string	&h = r.res.headers;
	std::string const crlf = std::string(CRLF);
	std::ostringstream	s;

	h += "Content-Type: " + r.file_type + crlf;
	if (!r.conf->chunked_transfer_encoding || r.is_json) {
		s << r.res.body.size();
		h += "Content-Length: " + s.str() + crlf;
	} else {
		h += "Transfer-Encoding: chunked" + crlf;
	}
	if (r.n_req <= 0) {
		h += "Connection: close" + crlf;
	} else {
		h += "Connection: keep-alive" + crlf;
	}
	if (r.err_code == 405) {
		h += "Accept:";
		for (std::set<std::string>::const_iterator it = r.location->methods.begin();
				it != r.location->methods.end(); it++) {
			h += " " + *it;
		}
		h += crlf;
	}
}
