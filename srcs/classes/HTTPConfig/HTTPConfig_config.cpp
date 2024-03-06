#include "HTTPConfig.hpp"

int HTTPConfig::configurate(std::string const path, std::string const config_file) {
    if (config_file.find(".conf") == std::string::npos) {
        std::string choice;
        std::cout << "Not a .conf file. Do you wish to continue ? (Y/N) > ";
        std::getline(std::cin, choice);
        if (choice != "Y" && choice != "y")
            return (1);
        else
            std::cout << "Proceeding..." << std::endl;
    }

    this->path = path;

    std::ifstream    config(config_file.c_str());
    if (!config || !config.good()) {
        std::cerr << "Not a valid config file (does not exist or is not readble)" << std::endl;
        return (1);
    }

    return (this->parse_infile(config));
}
