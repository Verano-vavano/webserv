#ifndef HTTPSERV_HPP
# define HTTPSERV_HPP

# include "HTTPConfig.hpp"
# include "HTTPProtocol.hpp"
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
		~HTTPServ(void);

		void socketsInit(void);
		void mainLoop(void);
		void socketsClose(void);

    private:

		typedef struct s_socket {
			int fd;
			int port;
			bool is_client;
			t_response_creator	rc;
		} t_socket;


		int						epoll_fd;
        HTTPConfig				conf;
		std::vector<t_socket>	sockets;

		HTTPConfig::t_config*	get_config_client(int port);
		t_response_creator&		get_client_config(std::vector<t_socket> &cl, int fd);
		t_socket				*find_socket(int fd);
		t_socket				initClientSocket(HTTPServ::t_socket server);
		void					event_change(int fd, EPOLL_EVENTS event);
		void					epollinTheSocket(int socket_fd);
};

#endif /* HTTPSERV_HPP */

