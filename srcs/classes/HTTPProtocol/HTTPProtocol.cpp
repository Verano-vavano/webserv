#include "HTTPProtocol.hpp"

HTTPProtocol::HTTPProtocol(void) {
	this->log_file.open(LOG_FILE);
	return ;
}

HTTPProtocol::~HTTPProtocol(void) {
	return ;
}
