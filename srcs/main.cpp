#include "HTTPServ.hpp"

int	main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Please enter a config file" << std::endl;
        return (1);
    }

    HTTPServ    A(argv);
	A.CreateSocket();
	return (0);
}

