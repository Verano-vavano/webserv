#ifndef USERS_HPP
# define USERS_HPP

#include <string>
#include <vector>
#include "HTTPProtocol.hpp"

class Users {
public:
	Users(void);
	~Users(void);
	void handle_user(t_response_creator &rc);
private:
	typedef struct s_user {
		std::string name;
		std::string password;
		std::string auth_key;
	} t_user;

	std::vector<t_user> users;

	t_user parse_json(std::string body_json);
	t_user *find_matching_user(t_user user);
};

#endif
