#ifndef HTTPSERV_HPP
# define HTTPSERV_HPP

# include "HTTPConfig.hpp"
# include <iostream>

class HTTPServ {

	public:
		HTTPServ(void);
        HTTPServ(char **conf);
        HTTPServ(char *path, char *config_file);
		HTTPServ(HTTPServ const & old);
		HTTPServ & operator=(HTTPServ const & rhs);
		~HTTPServ(void);

    private:
        HTTPConfig  conf;
};

#endif /* HTTPSERV_HPP */

