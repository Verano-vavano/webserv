#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP

# include <iostream>
# include <map>
# include <vector>
# include <stack>
# include <algorithm>
# include <cctype>
# include <cstring>
# include <fstream>

# define BUFFER_SIZE 4096

# define O_SPACE_MODE (1 << 0)
# define O_ERROR_STOP (1 << 1)
# define O_WARNING_AS_ERROR (1 << 2)

# define ISSPACE " \f\n\r\t\v"

class HTTPConfig {

	public:
		HTTPConfig(void);
        HTTPConfig(char const **conf);
        HTTPConfig(std::string const path, std::string const config_file);
		~HTTPConfig(void);

        int configurate(std::string const path, std::string const config_file);

    private:
        // CONFIG OPTIONS
        typedef std::map<std::string, std::string> t_map_str_str;
        typedef t_map_str_str t_type;
        typedef t_map_str_str t_header;

        typedef struct {
            std::string default_uri;
            std::string replacement;
            std::string index;
            bool        alias;
        }   t_location;

        typedef struct {
            std::vector<int>    codes;
            int                 response;
            std::string         uri;
        }   t_error;

        typedef struct {
            int                     port;
            std::string             server_name;
            bool                    absolute_redirect;
            bool                    chunked_transfer_encoding;
            long                    client_body_timeout;
            long                    client_body_buffer_size;
            short                   client_body_in_file_only;
            long                    client_header_buffer_size;
            long                    client_max_body_size;
            std::string             default_type;
            std::vector<t_error>    error_page;
            bool                    ignore_invalid_headers;
            long                    keepalive_requests;
            long                    keepalive_time;
            bool                    log_not_found;
            bool                    log_subrequest;
            t_type                  types;
            t_header                headers;
            std::vector<t_location> locations;
        }   t_config;

        bool                    set;
        t_config                default_config;
        std::vector<t_config>   servers;
        std::string             path;



        // CONFIG PARSER

        typedef struct {
            std::stack<std::string> blocks;
            bool                    in_http;
            t_config                *current_serv;
            int                     options;
            unsigned long           line;
        }   t_parser;

        int parse_infile(std::ifstream &f);

        int understand_the_line(char *buffer, t_parser &opt);
        int understand_the_cut(std::string & cut, t_parser &opt);

        static int set_define(std::string & cut, t_parser &opt);
        static int set_block(std::string & cut, t_parser &opt);
        static int set_other(std::string & cut, t_parser &opt);

        static std::pair<char, int>     search_delim(std::string const buffer, t_parser &opt);
        static std::string              trim_buffer(char *buffer);
        static void                     split_cut(std::vector<std::string> &s, std::string const & cut);
		static char*					skip_block(char *buffer, int start);

		static bool	warning(std::string const message, unsigned long line, int mask);
		static bool	error(std::string const message, unsigned long line, int mask);
};

#endif /* HTTPCONFIG_HPP */
