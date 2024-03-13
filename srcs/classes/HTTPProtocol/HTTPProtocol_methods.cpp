#include "HTTPProtocol.hpp"

void	HTTPProtocol::handle_get(t_response_creator &r) {
	std::cout << "Coucou je suis dans handle_get" << std::endl;
	std::string	&uri = r.req.uri;
	std::cout << "Coucou je suis apres setting the URI it is " << uri  << std::endl;
	if (uri.empty() || uri[0] != '/') {
		r.err_code = 400;
		return ;
	}

	std::string	file = get_complete_uri(uri, r.conf);
	std::cout << "So complete URI is " << file << std::endl;

	r.file_type = file.substr(file.find_last_of(".") + 1);
	std::cout << "Okk donc la je substr" << std::endl;

	std::ifstream	fs(file.c_str());
	std::cout << "Et la c'est mon beau ifstream" << std::endl;
	if (!fs || !fs.good()) {
		r.err_code = 404;
		return ;
	}
	std::cout << "Before lire the entire fichier!" << std::endl;
	this->read_entire_file(r.res.body, fs);
	std::cout << "Puis la j'ai fini de lire le file!" << std::endl;
}
