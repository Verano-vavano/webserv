#ifndef HTTPPROTOCOL_HPP
# define HTTPPROTOCOL_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>

# define CRLF "\r\n"

typedef struct {
	std::string											method;
	std::string											uri;
	std::map<std::string, std::vector<std::string> >	headers;
	std::string											body;
	bool												bad_request;
}	t_request;

typedef struct {
	std::string	status_line;
	std::string	headers;
	std::string	body;
}	t_response;


// A small machine capable of understanding a user-client request
// and also creating the adequate response according to a config file
class HTTPProtocol {

	public:
		HTTPProtocol(void) { return ; }
		~HTTPProtocol(void) { return ; }

		void	understand_request(t_request &req, std::string &s);
		void	print_request(t_request &req);
		void	create_response(t_response &res, t_request &req);

	private:
		std::vector<std::string>	split_header_val(std::string val);

};

#endif /* HTTPPROTOCOL_HPP */
