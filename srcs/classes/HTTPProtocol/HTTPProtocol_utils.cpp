#include "HTTPProtocol.hpp"

void	HTTPProtocol::read_entire_file(std::string &buf, std::ifstream &file) {
	std::stringstream	buffer;

	buffer << file.rdbuf();
	buf = buffer.str();
}
