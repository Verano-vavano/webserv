#ifndef HTTPPROTOCOL_HPP
# define HTTPPROTOCOL_HPP

# include "HTTPConfig.hpp"

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
}	t_request;

typedef struct {
	std::string	status_line;
	std::string	headers;
	std::string	body;
}	t_response;

typedef struct {
	t_request				req;
	t_response				res;
	HTTPConfig::t_config	*conf;
	int						err_code;
	std::string				file_type;
}	t_response_creator;


// A small machine capable of understanding a user-client request
// and also creating the adequate response according to a config file
class HTTPProtocol {

	public:
		HTTPProtocol(void) { return ; }
		~HTTPProtocol(void) { return ; }

		int			understand_request(t_request &req, std::string &s);
		void		print_request(t_request &req);

		void		create_response(t_response_creator &r);
		std::string	format_response(t_response &res);

	private:
		std::vector<std::string>	split_header_val(std::string val);

		void	handle_method(t_response_creator &r);
		void	handle_get(t_response_creator &r);

		std::string	get_dir_uri(std::string &uri, HTTPConfig::t_config *conf, int *err);

		void	handle_error_code(t_response_creator &r);

		void	set_headers(t_response_creator &r);

};

#endif /* HTTPPROTOCOL_HPP */
