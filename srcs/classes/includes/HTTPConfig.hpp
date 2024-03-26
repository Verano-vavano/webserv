#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP

# include <iostream>
# include <map>
# include <vector>
# include <stack>
# include <set>
# include <algorithm>
# include <cctype>
# include <cstring>
# include <fstream>
# include <cstdlib>
# include <limits>
# include <stdarg.h>

# define BUFFER_SIZE 4096

# define O_SPACE_MODE (1 << 0)
# define O_ERROR_STOP (1 << 1)
# define O_WARNING_AS_ERROR (1 << 2)
# define O_SILENT (1 << 3)
# define O_TOGGLE_BOOL (1 << 4)

# define ISSPACE " \f\n\r\t\v"

# define DEFAULT_PORT 8080
# define DEFAULT_NAME "localhost"
# define DEFAULT_LOCATION "./"
# define DEFAULT_REDIR true
# define DEFAULT_CHUNKED true
# define DEFAULT_BODY_TO 60
# define DEFAULT_BODY_BUF_SIZE 8000
# define DEFAULT_BODY_IN false
# define DEFAULT_HEAD_BUF_SIZE 1000
# define DEFAULT_MAX_BODY 100000
# define DEFAULT_TYPE "text/plain"
# define DEFAULT_IGNORE_INVALID true
# define DEFAULT_KEEPALIVE_REQ 1000
# define DEFAULT_KEEPALIVE_TIME 60
# define DEFAULT_LOG_NOT_FOUND true
# define DEFAULT_LOG_SUB false
# define DEFAULT_INTERPRETER true

class HTTPConfig {

	public:
		HTTPConfig(void);
		HTTPConfig(char const **conf);
		HTTPConfig(std::string const path, std::string const config_file);
		~HTTPConfig(void);

		void 	configurate(std::string const path, std::string const config_file);
		void	print_config(void) const;

		typedef std::map<std::string, std::string> t_map_str_str;
		typedef t_map_str_str t_type;
		typedef t_map_str_str t_header;

		typedef struct {
			std::set<std::string>				cgi_exec; // Vector of executables
			std::map<std::string, std::string>	cgi_interpreter; // Pairs extension / interpreter
		}	t_cgi;

		typedef struct s_location {
			std::string default_uri;
			std::string replacement;
			std::string index;
			t_cgi		cgi;
			bool		alias;
			struct s_location & operator=(struct s_location const & rhs);
		}   t_location;

		typedef struct s_error {
			std::set<int>	codes;
			int				 response;
			std::string		 uri;

			struct s_error & operator=(struct s_error const & rhs);
		}   t_error;

		typedef struct s_config {
			int					 port;
			std::string			 server_name;
			t_location				default_root;
			bool					absolute_redirect;
			bool					chunked_transfer_encoding;
			long					client_body_timeout;
			long					client_body_buffer_size;
			short				   client_body_in_file_only;
			long					client_header_buffer_size;
			long					client_max_body_size;
			std::string			 default_type;
			std::vector<t_error>	error_page;
			bool					ignore_invalid_headers;
			long					keepalive_requests;
			long					keepalive_time;
			bool					log_not_found;
			bool					log_subrequest;
		bool			default_interpreter;
			t_type				  types;
			t_header				headers;
			std::vector<t_location> locations;
			std::string				path;

			struct s_config & operator=(struct s_config const & rhs);
		}   t_config;

		t_config				default_config;
		std::vector<t_config>   servers;
		std::string			 path;

	private:
		// CONFIG OPTIONS
		void	set_default_config(void);

		static void	copy_map_strstr(t_map_str_str & n, t_map_str_str const & old);


		// CONFIG PARSER

		typedef struct {
			std::stack<std::string> blocks;
			bool					in_http;
			t_config				*current_serv;
			int					 options;
			unsigned long		   line;
		}   t_parser;

		bool	parse_infile(std::ifstream &f, bool space_mode);

		int understand_the_line(std::string buffer, std::string & temp, t_parser &opt);
		int understand_the_cut(std::string & cut, t_parser &opt);

		int set_define(std::string & cut, t_parser &opt);
		int set_type(std::string & cut, t_parser &opt);
		int set_block(std::string & cut, t_parser &opt);
		int set_other(std::string & cut, t_parser &opt);

		int	set_error_page(std::vector<std::string> &split, t_parser &opt);

		static std::pair<char, int>	 search_delim(std::string const buffer, t_parser &opt);
		static std::string			  trim_buffer(char *buffer);
		static std::string			  trim_buffer(std::string const & buffer);
		static void					 split_cut(std::vector<std::string> &s, std::string const & cut);
		static void						skip_block(std::string & buffer, int start);
		static bool						in(std::string const s, ...);
		static long						translate_time(std::string arg);
	static bool	boolean_switch(bool &var, t_parser const &opt, std::vector<std::string> const & split);

		static bool	warning(std::string const message, unsigned long line, int mask);
		static bool	error(std::string const message, unsigned long line, int mask);
		static bool	unknown_command_error(t_parser &opt) { return (error("Unknown command", opt.line, opt.options)); }


		// PRINTER
		static void	print_server(t_config const &s);
};

#endif /* HTTPCONFIG_HPP */
