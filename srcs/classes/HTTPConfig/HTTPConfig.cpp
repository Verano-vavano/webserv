#include "HTTPConfig.hpp"

HTTPConfig::HTTPConfig(void) {}

HTTPConfig::HTTPConfig(char const **conf) {
	this->configurate(conf[0], conf[1]);
    return ;
}

HTTPConfig::HTTPConfig(std::string const path, std::string const config_file) {
	this->configurate(path, config_file);
    return ;
}

HTTPConfig::~HTTPConfig(void) { return ; }
