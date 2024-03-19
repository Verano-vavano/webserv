#include "HTTPServ.hpp"

// Useful global variable opened in R/W on .launched
// If sigint received, 'I' written in it, stopping epoll_wait
// and assigning FD_CLOSED to the variable, escaping the main loop
// If change takes too much time and epoll_wait restarts, it will loop once more
static int	g_stop_fd = FD_NOT_OPEN;

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

void HTTPServ::epollinTheSocket(int socket_fd) {
	epoll_event ev;
	ev.data.fd = socket_fd;
	ev.events = EPOLLIN;
	epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);
}

void HTTPServ::socketsInit(void) {
	this->epoll_fd = epoll_create(1);
	if (this->epoll_fd == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}

	g_stop_fd = open(".launched", O_RDWR | O_CREAT, 0666);
	if (g_stop_fd == FD_ERROR) {
		std::cerr << "Failed to create .launched" << std::endl;
	}
	this->epollinTheSocket(g_stop_fd);

	std::vector<HTTPConfig::t_config>::iterator configs_it = this->conf.servers.begin();
	for (; configs_it != this->conf.servers.end(); configs_it++) {
		t_socket tmp;
		tmp.fd = socketOpen(*configs_it);
		tmp.port = configs_it->port;
		tmp.is_client = false;
		if (tmp.fd != -1)
			epollinTheSocket(tmp.fd);
		this->sockets.push_back(tmp);
	}
}

void HTTPServ::socketsClose(void) {
	std::vector<t_socket>::iterator sockets_it = this->sockets.begin();
	for(; sockets_it != this->sockets.end(); sockets_it++) {
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
	for(; sockets_it != this->sockets.end(); sockets_it++) {
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
	std::cout << "ACCEPTED NEW CLIENT (fd = " << newClientSocket.fd << ", port = " << newClientSocket.port << ")" << std::endl;
	newClientSocket.is_client = true;
	newClientSocket.rc.conf = get_config_client(newClientSocket.port);

	this->sockets.push_back(newClientSocket);
	return (newClientSocket);
}

void HTTPServ::event_change(int fd, EPOLL_EVENTS event) {
	epoll_event tmp;
	tmp.data.fd = fd;
	tmp.events = event;
	epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, fd, &tmp);
}

void	HTTPServ::sigint_handler(int signal) {
	(void) signal;
	if (g_stop_fd == FD_ERROR || g_stop_fd == FD_NOT_OPEN) { return ; }
	write(g_stop_fd, "I", 1);
	close(g_stop_fd);
	remove("./.launched");
	g_stop_fd = FD_CLOSED;
	return ;
}

void HTTPServ::mainLoop(void) {
	HTTPProtocol Http;
	t_response_creator	tmp;

	signal(SIGINT, this->sigint_handler);

	while (g_stop_fd != FD_CLOSED) {
		ulong sockets_count = this->sockets.size();
		ulong i = 0;
		epoll_event wait_events[sockets_count + 1];

		for (; i < sockets_count; i++)
			wait_events[i].data.fd = -1;

		std::cout << "WAIT START... ";
		epoll_wait(this->epoll_fd, wait_events, sockets_count + 1, -1);
		std::cout << "END WAIT" << std::endl;

		std::cout << "SOCKETS_COUNT = " << sockets_count + 1 << std::endl;
		for (i = 0; i < sockets_count + 1 && wait_events[i].data.fd != -1; i++){
			std::cout << g_stop_fd << " | " << wait_events[i].data.fd << std::endl;
			t_socket *matching_socket = find_socket(wait_events[i].data.fd);
			if (!matching_socket) {
				std::cout << "QUITTING" << std::endl;
				break;
			}
			if (matching_socket->is_client) {
				if (wait_events[i].events == EPOLLIN) {
					char buffer[1024] = { 0 };
					int	ret = recv(matching_socket->fd, buffer, sizeof(buffer), 0);
				   	if (ret == -1) {
						std::cout << "Could not read from client connection" << std::endl;
						exit(EXIT_FAILURE);
					} else if (ret == 0) {
						epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, matching_socket->fd, &wait_events[i]);
						std::vector<t_socket>::iterator	it = this->sockets.begin();
						for (; it != this->sockets.end() && &*it != matching_socket; it++) {}
						std::cout << "DELETED NEW CLIENT (fd = " << it->fd << ", port = " << it->port << ")" << std::endl;
						close(it->fd);
						this->sockets.erase(it);
						continue ;
					}
					std::cout << "RECEIVED REQUEST FROM " << matching_socket->fd << std::endl;
					std::string request(buffer);
					Http.understand_request(matching_socket->rc.req, request);
					//Http.print_request(matching_socket->rc.req);
					if (matching_socket->rc.req.method == "POST")
						this->users.handle_user(matching_socket->rc);
					Http.create_response(matching_socket->rc);
					event_change(matching_socket->fd, EPOLLOUT);
				} else if (wait_events[i].events == EPOLLOUT){
					std::string res = Http.format_response(matching_socket->rc.res);
					std::cout << "Answer will be " << std::endl << res << std::endl;
					send(matching_socket->fd, res.c_str(), res.size(), 0);
					event_change(matching_socket->fd, EPOLLIN);
				} else {
					std::cout << "Could not handle event" << std::endl;
					continue ;
				}
			} else {
				t_socket newClientSocket = initClientSocket(*matching_socket);
				epollinTheSocket(newClientSocket.fd);
			}
		}
	}
}

HTTPServ::~HTTPServ(void) { return ; }
