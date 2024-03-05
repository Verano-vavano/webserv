#ifndef HTTPSERV_HPP
# define HTTPSERV_HPP

# include <iostream>

class HTTPServ {

	public:
		HTTPServ(void);
        HTTPServ(char **conf);
        HTTPServ(char *path, char *config_file);
		HTTPServ(HTTPServ const & old);
		HTTPServ & operator=(HTTPServ const & rhs);
		~HTTPServ(void);
};

#endif /* HTTPSERV_HPP */

