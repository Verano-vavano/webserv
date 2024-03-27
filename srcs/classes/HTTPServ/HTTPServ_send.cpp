#include "HTTPServ.hpp"

void	HTTPServ::send_data(int fd, const char *data, size_t size) {
	send(fd, data, size, 0);
}


void	HTTPServ::send_chunked_response(int fd, t_response_creator const &rc) {
	std::string	to_send = rc.res.status_line + CRLF + rc.res.headers + CRLF;
	std::cout << to_send;
	send(fd, to_send.c_str(), to_send.size(), 0);
	for (unsigned int i = 0; i < rc.res.body.size(); i += 255) {
		size_t chunkSize = std::min(rc.res.body.size() - i, (size_t) 255); // taille du morceau
											 std::stringstream	out;
		out << std::hex << chunkSize << "\r\n"; // taille du morceau en hexadécimal
		out.write(rc.res.body.c_str() + i, chunkSize); // envoi du morceau
		out << "\r\n"; // ligne vide après chaque morceau
		to_send = std::string(out.str());
		std::cout << to_send;
		send(fd, to_send.c_str(), to_send.size(), 0);
	}
	to_send = "0\r\n\r\n";
	send(fd, to_send.c_str(), to_send.size(), 0);
}
