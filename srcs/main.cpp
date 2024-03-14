#include "HTTPServ.hpp"
#include <unistd.h>

int	main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Please enter a config file" << std::endl;
        return (1);
    }

    HTTPServ    Serv(argv);
	// open server sockets
	// epoll all sockets
	// Serv.socketsInit();
	Serv.socketsInit();
	Serv.mainLoop();
	Serv.socketsClose();
	// Serv.mainLoop();
	// Serv.socketsClose();
	// std::cout << "Before return" << std::endl;
	return (0);
}

