#include "HTTPConfig.hpp"

bool	HTTPConfig::check_config(void) {
	std::map<int, t_config*>	servers;
	servers[5] = &(this->default_config);
	(void) servers;
	return (0);
}
