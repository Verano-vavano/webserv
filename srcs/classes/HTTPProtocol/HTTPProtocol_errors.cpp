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
	}

	if (r.err_code != 200 && r.res.body.size() == 0) {
		r.res.body = "<!DOCTYPE html><body>[DEFAULT ERROR] " + get_error_tag(r.err_code) + "</body>";
	}
	return ;

}
