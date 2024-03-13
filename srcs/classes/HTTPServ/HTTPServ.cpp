#include "HTTPServ.hpp"
#include "HTTPProtocol.hpp"
#include <asm-generic/socket.h>
#include <cstdio>
#include <cstdlib>
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
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	int	a = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &a, sizeof(a));
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	// htons = machine into to network byte order int
	serverAddr.sin_port = htons(config.port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cout << "could not bind socket " << config.port << " ... :(" << std::endl;
		close(serverSocket);
		return (-1);
	}
	// int = number of request will be queued before refusing requests.
	if (listen(serverSocket, 5) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	return (serverSocket);
}

epoll_event epollTheSocket(int socket_fd, int epoll_fd) {
	epoll_event ev;
	ev.data.fd = socket_fd;
	ev.events = EPOLLIN;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);
	return (ev);
}

void HTTPServ::socketsInit(void) {
	HTTPProtocol Http;
	t_response_creator r;
	r.conf = &(this->conf.default_config);
	int i = 0;
	std::vector<HTTPConfig::t_config>::iterator servers_it = this->conf.servers.begin();
	int epoll_fd = epoll_create(1);
	if (epoll_fd == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}
	for (; servers_it != this->conf.servers.end(); servers_it++, i++) {
		sockets_fds.push_back(socketOpen(*servers_it));
		std::cout << i << " port " << servers_it->port << " with fd " << sockets_fds.back() << std::endl;
		if (sockets_fds.back() != -1) {
			epoll_events.push_back(epollTheSocket(sockets_fds.back(), epoll_fd));
		}
	}
	while (true) {
		std::cout << "Retour de boucle--" << std::endl;
		// init temp struct with fds of -1
		epoll_event events[epoll_events.size()];
		for (ulong i = 0; i < epoll_events.size(); i++)
			events[i].data.fd = -1;

		epoll_wait(epoll_fd, events, epoll_events.size(), -1);

		// iteration over events
		for (ulong i = 0; i < epoll_events.size() && events[i].data.fd != -1; i++){

			std::vector<int>::iterator sockets_fds_it =	sockets_fds.begin();

			// iteration over sockets_fds to find matching activity.
			for (; sockets_fds_it != sockets_fds.end(); sockets_fds_it++) {
				if (*sockets_fds_it == events[i].data.fd) {
					if (clients_fds.find(*sockets_fds_it) == clients_fds.end()) {
						std::cout << "ahhh mon fd est un serveur" << std::endl;
						int newClientSocket = accept(*sockets_fds_it, 0, 0);
						sockets_fds.push_back(newClientSocket);
						clients_fds.insert(newClientSocket);
						epoll_events.push_back(epollTheSocket(newClientSocket, epoll_fd));
					} else {
						if (events[i].events == EPOLLIN) {
							std::cout << "ahh EPOLLIN" << std::endl;
							char buffer[1024] = { 0 };
							if (recv(*sockets_fds_it, buffer, sizeof(buffer), 0) == -1) {
								std::cout << "Could not read from client connection" << std::endl;
								exit(EXIT_FAILURE);
							}
							std::string mdr(buffer);
							Http.understand_request(r.req, mdr);
							Http.print_request(r.req);
							std::cout << "After print_request" << std::endl;
							// TODO create response qui bug!!
							Http.create_response(r);
							std::cout << "After create_response" << std::endl;
							events[i].events = EPOLLOUT;
							epoll_ctl(epoll_fd, EPOLL_CTL_MOD, *sockets_fds_it, &events[i]);
							std::cout << "Modifying socket to EPOLL_OUT" << std::endl;
							std::cout << Http.format_response(r.res);
						} else {
							std::cout << "ahh EPOLLOUT" << std::endl;
							std::string formated_res = Http.format_response(r.res);
							send(*sockets_fds_it, formated_res.c_str(), formated_res.size(), 0);
							events[i].events = EPOLLIN;
							epoll_ctl(epoll_fd, EPOLL_CTL_MOD, *sockets_fds_it, &events[i]);
							// close(*sockets_fds_it);
						}
					}
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
