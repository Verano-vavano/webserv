#include "HTTPProtocol.hpp"

// Used for debug purposes
void	HTTPProtocol::print_request(t_request &req) {
	std::cout << "REQUEST SENT" << std::endl;
	std::cout << "Method = " << req.method << std::endl;
	std::cout << "URI = " << req.uri << std::endl;
	std::cout << "Headers :" << std::endl;
	for (std::map<std::string, std::vector<std::string> >::const_iterator it = req.headers.begin();
			it != req.headers.end(); it++) {
		std::cout << it->first << ":";
		for (std::vector<std::string>::const_iterator it2 = it->second.begin();
				it2 != it->second.end(); it2++) {
			std::cout << " " << *it2;
		}
		std::cout << std::endl;
	}
	std::cout << "Body :" << std::endl;
	std::cout << req.body;
}
