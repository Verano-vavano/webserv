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
#define MAX_EVENTS 10

HTTPServ::HTTPServ(void) { return ; }

HTTPServ::HTTPServ(char **conf) {
	this->conf.configurate(conf[0], conf[1]);
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
	std::vector<HTTPConfig::t_config>::iterator servers = this->conf.servers.begin();
	for (; servers != this->conf.servers.end(); servers++) {
		sockets_fds.push_back(socketOpen(*servers));
		if (sockets_fds.back() != -1)
			epoll_events.push_back(epollinTheSocket(sockets_fds.back(), epoll_fd));
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

t_response_creator&	HTTPServ::get_client_config(std::vector<t_client_config> &cl, int fd) {
	for (std::vector<t_client_config>::iterator  it = cl.begin(); it != cl.end(); it++) {
		if (it->fd == fd) { return (it->rc); }
	}
	return (cl[0].rc);
}

void HTTPServ::mainLoop(void) {
	HTTPProtocol Http;
	std::vector<t_client_config>	clients_struct;
	t_response_creator	tmp;
	while (true) {
		epoll_event events[epoll_events.size()];
		for (ulong i = 0; i < epoll_events.size(); i++)
			events[i].data.fd = -1;

		epoll_wait(epoll_fd, events, epoll_events.size(), -1);

		// iteration over events to and find matching socket
		for (ulong i = 0; i < epoll_events.size() && events[i].data.fd != -1; i++){
			if (clients_fds.find(events[i].data.fd) == clients_fds.end()) {
				struct sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				int newClientSocket = accept(events[i].data.fd, (struct sockaddr*)&client_addr, &client_addr_len);
				t_client_config	new_client;
				new_client.fd = newClientSocket;
				new_client.port = ntohs(client_addr.sin_port);
				new_client.rc.conf = get_config_client(new_client.port);
				clients_struct.push_back(new_client);
				sockets_fds.push_back(newClientSocket);
				clients_fds.insert(newClientSocket);
				epoll_events.push_back(epollinTheSocket(newClientSocket, epoll_fd));
			} else {
				t_response_creator	cl_conf = this->get_client_config(clients_struct, events[i].data.fd);
				if (events[i].events & EPOLLIN) {
					char buffer[1024] = { 0 };
					if (recv(events[i].data.fd, buffer, sizeof(buffer), 0) == -1) {
						std::cout << "Could not read from client connection" << std::endl;
						exit(EXIT_FAILURE);
					}
					std::string mdr(buffer);
					Http.understand_request(cl_conf.req, mdr);
					std::cout << "Got request" << std::endl;
					Http.print_request(cl_conf.req);
					Http.create_response(cl_conf);
					events[i].events = EPOLLOUT;
					epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &events[i]);
					Http.format_response(cl_conf.res);
				} else if (events[i].events & EPOLLOUT) {
					std::string formated_res = Http.format_response(cl_conf.res);
					std::cout << formated_res << std::endl;
					send(events[i].data.fd, formated_res.c_str(), formated_res.size(), 0);
					events[i].events = EPOLLIN;
					epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &events[i]);
				} else {
					std::cout << events[i].events << std::endl;
					std::cout << "Error could not handle socket event" << std::endl;
				}
			}
		}
	}
}

void HTTPServ::socketsClose(void) {
	for (int i = 0; sockets_fds[i]; i++) {
		if (sockets_fds[i] != -1)
			close(sockets_fds[i]);
	}
}


HTTPServ::~HTTPServ(void) { return ; }
