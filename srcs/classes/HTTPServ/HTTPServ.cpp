#include "HTTPServ.hpp"
#include "HTTPProtocol.hpp"
#include <asm-generic/socket.h>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>

HTTPServ::HTTPServ(void) { return ; }

HTTPServ::HTTPServ(char **conf) {
	this->conf.configurate(conf[0], conf[1]);
	this->conf.print_config();
}

int socketOpen(HTTPConfig::t_config config) {
	// AF_INET = ipv4
	//  SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams.
	//  0 is the protocol, auto to tcp
	int newSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	// htons = machine into to network byte order int
	serverAddr.sin_port = htons(config.port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	int	a = 1;
	setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &a, sizeof(a));
	if (bind(newSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cout << "could not bind socket " << config.port << " ... :(" << std::endl;
		close(newSocket);
		return (-1);
	}
	// int = number of request will be queued before refusing requests.
	if (listen(newSocket, 5) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	return (newSocket);
}

epoll_event epollinTheSocket(int socket_fd, int epoll_fd) {
	epoll_event ev;
	ev.data.fd = socket_fd;
	ev.events = EPOLLIN;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);
	return (ev);
}

void HTTPServ::socketsInit(void) {
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}
	std::vector<HTTPConfig::t_config>::iterator configs_it = this->conf.servers.begin();
	for (; configs_it != this->conf.servers.end(); configs_it++) {
		t_socket tmp;
		tmp.fd = socketOpen(*configs_it);
		tmp.port = configs_it->port;
		tmp.is_client = false;
		sockets.push_back(tmp);
		if (sockets.back().fd != -1) {
			epollinTheSocket(sockets.back().fd, epoll_fd);
		}
	}
	std::vector<t_socket>::iterator sockets_it = sockets.begin();
	for(; sockets_it != sockets.end(); sockets_it++) {
		std::cout << "---Printing a socket---" << std::endl << std::endl;
		std::cout << "socket_fd = " << sockets_it->fd << " is_client = " <<sockets_it->is_client << std::endl;
	}
}

void HTTPServ::socketsClose(void) {
	std::vector<t_socket>::iterator sockets_it = sockets.begin();
	for(; sockets_it != sockets.end(); sockets_it++) {
		if (sockets_it->fd != -1) {
			std::cout << "closing socket " << sockets_it->fd << std::endl;
			close(sockets_it->fd);
		}
	}
}

HTTPConfig::t_config*	HTTPServ::get_config_client(int port) {
	if (this->conf.servers.size() == 0)
		return (&this->conf.default_config);
	for (std::vector<HTTPConfig::t_config>::iterator it = this->conf.servers.begin(); it != this->conf.servers.end(); it++) {
		if (it->port == port) { return (&*it); }
	}
	return (&this->conf.default_config);
}

t_response_creator&	HTTPServ::get_client_config(std::vector<t_socket> &cl, int fd) {
	for (std::vector<t_socket>::iterator  it = cl.begin(); it != cl.end(); it++) {
		if (it->fd == fd) { return (it->rc); }
	}
	return (cl[0].rc);
}

HTTPServ::t_socket *HTTPServ::find_socket(int fd) {
	std::vector<t_socket>::iterator sockets_it = this->sockets.begin();
	for(; sockets_it != sockets.end(); sockets_it++) {
		if (sockets_it->fd == fd)
			return (&*sockets_it);
	}
	return (NULL);
}

HTTPServ::t_socket HTTPServ::initClientSocket(HTTPServ::t_socket server) {
	t_socket newClientSocket;
	sockaddr_in client_addr;
	socklen_t sock_addr_len = sizeof(client_addr);

	newClientSocket.fd = accept(server.fd, (sockaddr*)&client_addr, &sock_addr_len);
	newClientSocket.port = server.port;
	std::cout << "\t new client socket  = " << newClientSocket.port << std::endl;
	newClientSocket.is_client = true;
	newClientSocket.rc.conf = get_config_client(newClientSocket.port);

	this->sockets.push_back(newClientSocket);
	return (newClientSocket);
}

void HTTPServ::event_change(int fd, EPOLL_EVENTS event) {
	epoll_event tmp;
	tmp.data.fd = fd;
	epoll_ctl(this->epoll_fd, event, fd, &tmp);
}

void HTTPServ::mainLoop(void) {
	HTTPProtocol Http;
	t_response_creator	tmp;

	while (true) {
		ulong sockets_count = sockets.size();
		ulong i = 0;
		epoll_event wait_events[sockets_count];

		for (; i < sockets_count; i++)
			wait_events[i].data.fd = -1;

		epoll_wait(epoll_fd, wait_events, sockets.size(), -1);

		for (i = 0; i < sockets_count && wait_events[i].data.fd != -1; i++){
			t_socket *matching_socket = find_socket(wait_events[i].data.fd);
			if (!matching_socket)
				break;
			if (matching_socket->is_client) {
				if (wait_events[i].events == EPOLLIN) {
					char buffer[1024] = { 0 };
					if (recv(matching_socket->fd, buffer, sizeof(buffer), 0) == -1) {
						std::cout << "Could not read from client connection" << std::endl;
						exit(EXIT_FAILURE);
					}
					std::string request(buffer);
					Http.understand_request(matching_socket->rc.req, request);
					Http.print_request(matching_socket->rc.req);
					Http.create_response(matching_socket->rc);
					event_change(matching_socket->fd, EPOLLOUT);
					Http.format_response(matching_socket->rc.res);
				} else if (wait_events[i].events == EPOLLOUT){
					std::string res = Http.format_response(matching_socket->rc.res);
					std::cout << res << std::endl;
					send(matching_socket->fd, res.c_str(), res.size(), 0);
					event_change(matching_socket->fd, EPOLLIN);
				} else {
					std::cout << "Could not handle event" << std::endl;
				}
			} else {
				t_socket newClientSocket = initClientSocket(*matching_socket);
				epollinTheSocket(newClientSocket.fd, this->epoll_fd);
			}
		}
	}
}

HTTPServ::~HTTPServ(void) { return ; }
