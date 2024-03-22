#ifndef HTTPPROTOCOL_HPP
# define HTTPPROTOCOL_HPP

# include "HTTPConfig.hpp"

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>
# include <unistd.h>
# include <sys/wait.h>

// DEFAULT TYPES
# define HTML	"text/html"
# define CSS	"text/css"
# define JS		"application/javascript"
# define WEBP	"image/webp"

# define CGI_TO	5

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

typedef struct {
	std::string file;
	HTTPConfig::t_cgi const *cgi;
} t_uri_cgi;


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

		static std::string	remove_useless_slashes(std::string const &uri);

	private:
		std::vector<std::string>	split_header_val(std::string val);

		void	handle_method(t_response_creator &r);
		void	handle_get(t_response_creator &r);


		void	check_type(t_response_creator &r);

		void	handle_error_code(t_response_creator &r);

		void	set_headers(t_response_creator &r);

		HTTPConfig::t_location	const &get_dir_uri(std::string const &uri, HTTPConfig::t_config *conf);
		t_uri_cgi	const	get_complete_uri(std::string const &uri, HTTPConfig::t_config *conf);
		void				get_body(std::string const &uri, t_response_creator &r, int change);
		std::string	const	get_mime_type(HTTPConfig::t_config *config, std::string &file_type);
		static std::string		get_error_tag(int err_code);

		static bool exec_cgi(std::string file, std::string *interpreter, t_response_creator &r);

		static void	read_entire_file(std::string &buf, std::ifstream &file);
		static bool		is_wildcard_match(std::string const & input, std::string const & match);

};

#endif /* HTTPPROTOCOL_HPP */
