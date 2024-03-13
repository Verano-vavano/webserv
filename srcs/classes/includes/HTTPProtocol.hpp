#ifndef HTTPPROTOCOL_HPP
# define HTTPPROTOCOL_HPP

# include "HTTPConfig.hpp"

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>

// DEFAULT TYPES
# define HTML	"text/html"
# define CSS	"text/css"
# define JS		"application/javascript"
# define WEBP	"image/webp"

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

		void	handle_error_code(t_response_creator &r);

		void	set_headers(t_response_creator &r);

		HTTPConfig::t_location	const get_dir_uri(std::string const &uri, HTTPConfig::t_config *conf);
		std::string				const get_complete_uri(std::string const &uri, HTTPConfig::t_config *conf);
		std::string				const get_mime_type(HTTPConfig::t_config *config, std::string &file_type);
		static std::string		get_error_tag(int err_code);

		static void	read_entire_file(std::string &buf, std::ifstream &file);

};

#endif /* HTTPPROTOCOL_HPP */
