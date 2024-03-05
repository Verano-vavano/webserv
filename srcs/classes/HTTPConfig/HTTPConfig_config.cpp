#include "HTTPConfig.hpp"

int HTTPConfig::configurate(std::string const path, std::string const config_file) {
    if (config_file.find(".conf") == std::string::npos) {
        std::string choice;
        std::cout << "Not a .conf file. Do you wish to continue ? (Y/N) > ";
        std::getline(std::cin, choice);
        if (choice != "Y" && choice != "y")
            return (1);
    }

    std::cout << "Proceeding..." << std::endl;

    (void) path;
    return (0);
}
