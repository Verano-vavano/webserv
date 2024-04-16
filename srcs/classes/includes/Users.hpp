#ifndef USERS_HPP
# define USERS_HPP

# include <string>
# include <vector>
# include <cstdio>
# include <cstdlib>
# include <sstream>
# include <stdlib.h>
# include <time.h>

# include "response_creator.hpp"

class Users {
public:
	Users(void);
	~Users(void);

	void	handle_post(t_response_creator &rc);
	void	handle_del(t_response_creator &rc);
	void	save_sessions(void) const;
private:
	typedef struct s_user {
		std::string name;
		std::string password;
	} t_user;

	typedef struct s_session {
		std::string	auth;
		std::string name;
	} t_session;

	std::vector<t_user>		users;
	std::vector<t_session>	sessions;

	static t_user parse_json_post(int &err_code, std::string body_json);
	t_user *find_matching_user(t_user user);
	void	create_new_user(t_response_creator &rc, t_user& user);
	static void	add_session(t_session &user);
	void	delete_user_auth(std::string body_json);
};

# include "HTTPProtocol.hpp"

#endif
