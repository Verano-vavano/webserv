#ifndef HTTPSERV_HPP
# define HTTPSERV_HPP

# include "HTTPConfig.hpp"
# include <iostream>
#include <sys/epoll.h>
#include <vector>

class HTTPServ {

	public:
		HTTPServ(void);
        HTTPServ(char **conf);
        HTTPServ(char *path, char *config_file);
		HTTPServ(HTTPServ const & old);
		HTTPServ & operator=(HTTPServ const & rhs);
		void socketsInit(void);
		void socketsClose(void);
		~HTTPServ(void);

    private:
        HTTPConfig  conf;
		std::vector<int> sockets_fds;
	std::vector<epoll_event> epoll_events;
};

#endif /* HTTPSERV_HPP */

