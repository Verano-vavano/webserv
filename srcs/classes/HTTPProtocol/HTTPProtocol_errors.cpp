#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_error_code(t_response_creator &r) {
	//std::cout << "ERROR CODE = " << r.err_code << std::endl;

	std::vector<HTTPConfig::t_error>	&errors = r.conf->error_page;
	std::vector<HTTPConfig::t_error>::const_iterator it;
	bool	found = false;

	for (it = errors.begin(); it != errors.end(); it++) {
		for (std::set<int>::const_iterator it2 = it->codes.begin();
				it2 != it->codes.end(); it2++) {
			if (*it2 == r.err_code) { found = true; break ; }
		}
		if (found) { break ; }
	}

	if (found) {
		get_body(it->uri, r, it->response);
	} else if (r.err_code != 200) {
		r.res.body = "[DEFAULT ERROR] " + get_error_tag(r.err_code);
	}
	return ;

}

std::string HTTPProtocol::get_error_tag(int err_code) {
	std::ostringstream	s;
	s << err_code;
	std::string	err_mes;
	switch (err_code) {
	//info
		case 100: err_mes = "continue";
			break;
		case 101: err_mes = "switching protocols";
			break;
		case 102: err_mes = "processing";
			break;
		case 103: err_mes = "early hints";
			break;
	//success
		case 200: err_mes = "OK";
			break;
		case 201: err_mes = "created";
			break;
		case 202: err_mes = "accepted";
			break;
		case 203: err_mes = "non-autoritative information";
			break;
		case 204: err_mes = "no content";
			break;
		case 205: err_mes = "reset content";
			break;
		case 206: err_mes = "partial content";
			break;
		case 207: err_mes = "multi status";
			break;
		case 208: err_mes = "Already Reported";
			break;
		case 226: err_mes = "IM used";
			break;
	//redirection
		case 300: err_mes = "Multiple Choices";
			break;
		case 301: err_mes = "moved permanantly";
			break;
		case 302: err_mes = "found";
			break;
		case 303: err_mes = "See Other";
			break;
		case 304: err_mes = "not modified";
			break;
		case 305: err_mes = "Use proxy";
			break;
		case 306: err_mes = "unused";
			break;
		case 307: err_mes = "temporary redirect";
			break;
		case 308: err_mes = "Permanent redirect";
			break;
	//bad request
		case 400: err_mes = "Bad request";
			break;
		case 401: err_mes = "Unauthorized";
			break;
		case 402: err_mes = "Payment required";
			break;
		case 403: err_mes = "forbidden";
			break;
		case 404: err_mes = "not found";
			break;
		case 405: err_mes = "methode not allowed";
			break;
		case 406: err_mes = "Not acceptable";
			break;
		case 407: err_mes = "proxy authentification required";
			break;
		case 408: err_mes = "request timeout";
			break;
		case 409: err_mes = "Conflict";
			break;
		case 410: err_mes = "Gone";
			break;
		case 411: err_mes = "Length Required";
			break;
		case 412: err_mes = "Precondition failed";
			break;
		case 413: err_mes = "payload too large";
			break;
		case 414: err_mes = "URL too long";
			break;
		case 415: err_mes = "Unsuported Media type";
			break;
		case 416: err_mes = "range not satisfied";
			break;
		case 417: err_mes = "expectation failed";
			break;
		case 418: err_mes = "I'm a teapot";
			break;
		case 421: err_mes = "misdirected request";
			break;
		case 422: err_mes = "Unprocessable content";
			break;
		case 423: err_mes = "locked";
			break;
		case 424: err_mes = "failed dependency";
			break;
		case 425: err_mes = "too early";
			break;
		case 426: err_mes = "Upgrade required";
			break;
		case 428: err_mes = "precondition required";
			break;
		case 429: err_mes = "too many request";
			break;
		case 431: err_mes = "request Header fileds too large";
			break;
		case 451: err_mes = "Unavailable for legal reasons";
			break;
	//server error
		case 500: err_mes = "internal server error";
			break;
		case 501: err_mes = "not implemented";
			break;
		case 502: err_mes = "Bad Gateway";
			break;
		case 503: err_mes = "service unavailable";
			break;
		case 504: err_mes = "Gateway Timeout";
			break;
		case 505: err_mes = "HTTP version not suported";
			break;
		case 506: err_mes = "variant also negotiate";
			break;
		case 507: err_mes = "Insufficiant storage";
			break;
		case 508: err_mes = "Loop detected";
			break;
		case 510: err_mes = "not extended";
			break;
		case 511: err_mes = "Network Authentification Required";
			break;
		default: err_mes = "Unknown";
	}
	return (s.str() + " " + err_mes);
}
