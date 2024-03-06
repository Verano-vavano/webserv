#include "HTTPServ.hpp"

HTTPServ::HTTPServ(void) { return ; }

HTTPServ::HTTPServ(char **conf) {
	std::cout << this->conf.configurate(conf[0], conf[1]) << std::endl;
    return ;
}

HTTPServ::~HTTPServ(void) { return ; }
