#ifndef HTTPSERV_HPP
# define HTTPSERV_HPP

# include "HTTPConfig.hpp"
# include "HTTPProtocol.hpp"
# include <iostream>
#include <set>
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
		void mainLoop(void);
		void socketsClose(void);
		~HTTPServ(void);

    private:
		typedef struct {
			int					fd;
			int					port;
			t_response_creator	rc;
		}	t_client_config;
		int			epoll_fd;
        HTTPConfig  conf;
		std::vector<int> sockets_fds;
		std::set<int> clients_fds;
		std::vector<epoll_event> epoll_events;

		HTTPConfig::t_config*	get_config_client(int port);
		t_response_creator&	get_client_config(std::vector<t_client_config> &cl, int fd);
};

#endif /* HTTPSERV_HPP */

