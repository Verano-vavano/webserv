#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_get(t_response_creator &r) {
	std::string	&uri = r.req.uri;
	if (uri.empty() || uri[0] != '/') {
		r.err_code = 400;
		return ;
	}

	HTTPConfig::t_location	dir = get_dir_uri(uri, r.conf);
	std::string	file = uri.substr(uri.find_last_of("/"));
	std::cout << file << std::endl;

	std::string	filewdir;
	if (file == "/") {
		filewdir = dir.replacement + "/" + dir.index;
	}
	else
		filewdir = dir.replacement + "/" + uri;

	std::ifstream	fs(filewdir.c_str());
	if (!fs || !fs.good()) {
		r.err_code = 404;
		return ;
	}
	char			buff[BUFFER_SIZE];
	while (!fs.eof()) {
		fs.read(buff, BUFFER_SIZE - 1);
		buff[fs.gcount()] = 0;
		r.res.body += buff;
	}
}
