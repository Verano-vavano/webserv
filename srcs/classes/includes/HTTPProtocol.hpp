#ifndef HTTPPROTOCOL_HPP
# define HTTPPROTOCOL_HPP

# include "HTTPConfig.hpp"
# include "HTTPDefines.hpp"
# include "Users.hpp"
# include "response_creator.hpp"

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>
# include <unistd.h>
# include <climits>
# include <ctime>
# include <sys/wait.h>

// DIRECTORY LISTING
# include <sys/stat.h>
# include <dirent.h>

# define CGI_TO	5
# define CGI_LOOP 200
# define CGI_SLEEP CGI_TO * 1000000 / CGI_LOOP

# define CRLF "\r\n"
# define DIV_END "\r\n\r\n"
# define CHUNK_END "0\r\n\r\n"

# define LOG_FILE "miniweb.log"


// A small machine capable of understanding a user-client request
// and also creating the adequate response according to a config file
class HTTPProtocol {

	public:
		HTTPProtocol(void) { return ; }
		~HTTPProtocol(void) { return ; }

		int		read_and_understand_request(int fd, t_response_creator &r);
		void		print_request(t_request &req);

		void		create_response(t_response_creator &r);
		std::string	format_response(t_response &res);

		void				save_user_session(void) const;
		static std::string	remove_useless_slashes(std::string const &uri);
		static void	empty_request(t_request &req);

	private:
		Users		user_manager;

		// PARSER
		static bool	check_div_end(std::string const & buf);
		std::vector<std::string>	split_header_val(std::string val);
		void	parse_headers(std::string & s, t_response_creator & r);
		static void	empty_fd_in(int fd);
		short		read_crlfcrlf(int fd, t_response_creator &r, long buf_size, std::string & req, unsigned long length);

		void	handle_method(t_response_creator &r);
		void	handle_get(t_response_creator &r);
		void	handle_post(t_response_creator &r);
		void	handle_delete(t_response_creator &r);

		bool	get_body(std::string const &uri, t_response_creator &r, int change);

		void	check_type(t_response_creator &r);

		void	handle_error_code(t_response_creator &r);

		void	set_headers(t_response_creator &r);

		HTTPConfig::t_location	&get_dir_uri(std::string const &uri, HTTPConfig::t_config *conf);
		static void	get_complete_uri(t_response_creator &r, std::string const &uri);
		static void	directory_listing(t_response_creator &r, std::string const & dir, std::string const &uri);
		std::string	const	get_mime_type(HTTPConfig::t_config *config, std::string &file_type);
		static std::string		get_error_tag(int err_code);

		void		cgi(t_response_creator &r) const;
		static bool exec_cgi(std::string file, std::string *interpreter, t_response_creator &r);
		static std::string *get_default_interpreter(std::string const & file_type);

		static void	read_entire_file(std::string &buf, std::ifstream &file);
		static bool		is_wildcard_match(std::string const & input, std::string const & match);
		static bool	is_directory(std::string const & file);


		bool		body_too_large(t_request& req, size_t size_max);
		static void	get_file_type(t_response_creator &r);

		void	log_it(t_response_creator &r);

};

#endif /* HTTPPROTOCOL_HPP */
