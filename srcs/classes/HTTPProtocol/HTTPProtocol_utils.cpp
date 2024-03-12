#include "HTTPProtocol.hpp"

void	HTTPProtocol::read_entire_file(std::string &buf, std::ifstream &file) {
	char			buff[BUFFER_SIZE];

	while (!file.eof()) {
		file.read(buff, BUFFER_SIZE - 1);
		buff[file.gcount()] = 0;
		buf += buff;
	}
}
