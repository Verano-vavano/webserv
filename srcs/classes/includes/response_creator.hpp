#ifndef REPONSE_CREATOR_HPP
# define REPONSE_CREATOR_HPP

# include "HTTPConfig.hpp"

typedef struct {
	std::string											method;
	std::string											uri;
	std::string											http_version;
	std::map<std::string, std::vector<std::string> >	headers;
	std::string											body;
	bool												content_is_type(std::string type);
}	t_request;

typedef struct {
	std::string	status_line;
	std::string	headers;
	std::string	body;
}	t_response;

typedef struct {
	t_request				req;
	t_response				res;
	std::string				better_uri;
	HTTPConfig::t_config	*conf;
	HTTPConfig::t_location const	*location;
	int						err_code;
	std::string				file_type;
	std::string				file;
	long	n_req;
}	t_response_creator;

#endif /* REPONSE_CREATOR_HPP */
