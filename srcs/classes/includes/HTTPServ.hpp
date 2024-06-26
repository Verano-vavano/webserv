#ifndef HTTPSERV_HPP
# define HTTPSERV_HPP

# include "HTTPConfig.hpp"
# include "HTTPProtocol.hpp"
# include "Users.hpp"
#include <unistd.h>
# include <arpa/inet.h>
#include <netinet/in.h> // sockaddr_in
# include <iostream>
# include <fcntl.h>
# include <signal.h>
#include <sys/epoll.h>
#include <vector>

# define FD_NOT_OPEN 0
# define FD_ERROR -1
# define FD_CLOSED -2
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

		typedef struct s_socket {
			int 								fd;
			int 								port;
			bool 								is_client;
			t_response_creator					rc;
			std::vector<HTTPConfig::t_config *>	possible_config;
		} t_socket;


	private:

		int						epoll_fd;
		HTTPConfig				conf;
		std::vector<t_socket>	sockets;

		int						socketOpen(HTTPConfig::t_config config);
		void					get_config_client(HTTPServ::t_socket &sock, int port);
		t_response_creator&		get_client_config(std::vector<t_socket> &cl, int fd);
		t_socket				*find_socket(int fd);
		t_socket				initClientSocket(HTTPServ::t_socket server);
		void					event_change(int fd, unsigned int event);
		void					delete_client(t_socket *matching_socket, epoll_event *ev);
		void					epollinTheSocket(int socket_fd);

		static short	send_data(int fd, const char *data, size_t size);
		static short	send_chunked_response(int fd, t_response_creator const & rc);
		static bool		send_chunked_file_content(int fd, t_response_creator const & rc);
		static bool		send_chunked_body_content(int fd, t_response_creator const & rc);

		static void				sigint_handler(int signal);

};

# include "Logger.hpp"

#endif /* HTTPSERV_HPP */

