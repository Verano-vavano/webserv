#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP

# include <iostream>

class HTTPConfig {

	public:
		HTTPConfig(void);
        HTTPConfig(char const **conf);
        HTTPConfig(std::string const path, std::string const config_file);
		HTTPConfig(HTTPConfig const & old);
		HTTPConfig & operator=(HTTPConfig const & rhs);
		~HTTPConfig(void);

        int configurate(std::string const path, std::string const config_file);

    private:
        bool    set;
};

#endif /* HTTPCONFIG_HPP */

