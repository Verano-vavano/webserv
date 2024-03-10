#include "HTTPServ.hpp"
#include <asm-generic/socket.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#define MAX_EVENTS 10

HTTPServ::HTTPServ(void) { return ; }

HTTPServ::HTTPServ(char **conf) {
	std::cout << this->conf.configurate(conf[0], conf[1]) << std::endl;
	this->conf.print_config();
    return ;
}

void HTTPServ::CreateSocket(void) {
	// AF_INET = ipv4
	//  SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams.
	//  0 is the protocol, auto to tcp
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, 0, sizeof(int));
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	// htons = machine into to network byte order int
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cout << "could not bind socket.... :(" << std::endl;
		return ;
	}
	// int = number of request will be queued before refusing requests.
	if (listen(serverSocket, 5) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	int epoll_fd = epoll_create(1);
	if (epoll_fd == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}
	struct epoll_event ev, events[MAX_EVENTS];
	ev.data.fd = serverSocket;
	ev.events = EPOLLIN;
	events[0] = ev;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &ev);
	int wait_fds = epoll_wait(epoll_fd, &ev, MAX_EVENTS, -1);
	std::cout << "after epoll wait" << std::endl;
	if (wait_fds == -1) {
		perror("NFDS FAILURE");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < MAX_EVENTS; ++i){
		std::cout << "Entering loop " << i << std::endl;
		std::cout << "fd is " << events[i].data.fd << std::endl;
		if (events[i].data.fd == serverSocket) {
			std::cout << "before accept" << std::endl;
			int clientSocket = accept(serverSocket, 0, 0);
			std::cout << "after accept" << std::endl;
			std::cout << "accepted client into" << clientSocket << std::endl;
			char buffer[1024] = { 0 };
			// Read N bytes from socket FD
			if (recv(clientSocket, buffer, sizeof(buffer), 0) == -1) {
				std::cout << "Could not read from socket" << std::endl;
			}
			std::cout << "got: " << buffer << std::endl;
			const char *res = "HTTP/1.1 200 OK\n\n<h1>salut mdr</h1>";
			send(clientSocket, res, strlen(res), 0);
			close(clientSocket);
		}
	}
	// wait for a connection on socket FD
	close(epoll_fd);
	close(serverSocket);
	return ;
}

HTTPServ::~HTTPServ(void) { return ; }
