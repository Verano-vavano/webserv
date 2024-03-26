#include "HTTPServ.hpp"
#include <unistd.h>

int	main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Please enter a config file" << std::endl;
		return (1);
	}

	HTTPServ	Serv(argv);

	Serv.socketsInit();
	Serv.mainLoop();
	Serv.socketsClose();

	return (0);
}

