#include "HTTPServ.hpp"

void	HTTPServ::send_data(int fd, const char *data, size_t size) {
	send(fd, data, size, 0);
}

/* Partitionning the body content into chunks */
void	HTTPServ::send_chunked_body_content(int fd, t_response_creator const & rc) {
	std::string		to_send;
	std::stringstream	out;

	for (unsigned int i = 0; i < rc.res.body.size(); i += 255) {
		size_t chunkSize = std::min(rc.res.body.size() - i, (size_t) 255);
		out << std::hex << chunkSize << "\r\n";
		out.write(rc.res.body.c_str() + i, chunkSize);
		out << "\r\n";
		to_send = std::string(out.str());
		send_data(fd, to_send.c_str(), to_send.size());
	
		out.clear();
		out.str(std::string()); // Clear stringstream
	}
}

/* Reading the file chunk by chunk and sending it each time */
void	HTTPServ::send_chunked_file_content(int fd, t_response_creator const & rc) {
	std::ifstream		file(rc.file.c_str(), std::ios::binary);
	std::string		to_send;
	std::stringstream	out;

	if (!file || !file.good()) { return ; }
	while (!file.eof()) {
		std::vector<char> buffer(255);
		file.read(buffer.data(), buffer.size());
		size_t bytesRead = file.gcount();

		out << std::hex << bytesRead << "\r\n";

		out.write(buffer.data(), bytesRead);
		out << "\r\n";
		to_send = std::string(out.str());
		send_data(fd, to_send.c_str(), to_send.size());

		out.clear();
		out.str(std::string());
	}
}

void	HTTPServ::send_chunked_response(int fd, t_response_creator const &rc) {
	std::string	to_send = rc.res.status_line + CRLF + rc.res.headers + CRLF;
	std::cout << to_send;
	send_data(fd, to_send.c_str(), to_send.size());
	if (rc.file == "") {
		send_chunked_body_content(fd, rc);
	} else {
		send_chunked_file_content(fd, rc);
	}
	to_send = "0\r\n\r\n";
	send(fd, to_send.c_str(), to_send.size(), 0);
}
