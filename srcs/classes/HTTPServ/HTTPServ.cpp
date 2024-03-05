#include "HTTPServ.hpp"

HTTPServ::HTTPServ(void) { return ; }

HTTPServ::HTTPServ(char **conf) {
    this->conf.configurate(conf[0], conf[1]);
    return ;
}

HTTPServ::~HTTPServ(void) { return ; }
