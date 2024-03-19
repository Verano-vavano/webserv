#include "Users.hpp"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdlib.h>
#include <time.h>

Users::Users(void) {};

Users::~Users(void) {};

Users::t_user Users::parse_json(std::string json) {
	t_user recieved_user;
	size_t user_begin = json.find("\":\"") + 3;
	size_t user_end = json.find(',') - user_begin - 1;
	size_t pass_begin = json.find(',') + 13;
	size_t pass_end = json.size() - pass_begin - 2;

	recieved_user.name = json.substr(user_begin, user_end);
	recieved_user.password = json.substr(pass_begin, pass_end);
	return (recieved_user);
}

std::string trim_body(std::string body) {
	ulong i = 0;
	for (; body[i] && (isspace(body[i]) || body[i] == '\n'); i++);
	ulong j = i;
	for (; body[j] && body[j] != '\n';j++);
	return (body.substr(i, j));
}

std::string format_json_line(std::string key, std::string val, bool last_line) {
	std::string line('"' + key  + '"' + ':' + '"' + val + '"');
	if (last_line) {
		line += '\n';
	} else {
		line += ",\n";
	}
	return line;
}

Users::t_user *Users::find_matching_user(t_user user) {
	t_user matching_user;
	std::vector<t_user>::iterator users_it = this->users.begin();
	for (; users_it != this->users.end(); users_it++) {
		if (users_it->name == user.name){
			return (&*users_it);
		}
	}
	return(NULL);
}

std::string generate_cookie(void) {
	std::stringstream ret;
	srand(time(NULL));
	ret << rand();
	return (ret.str());
}
void Users::handle_user(t_response_creator &rc) {
	t_user recieved = this->parse_json(trim_body(rc.req.body));
	t_user *matching_user = find_matching_user(recieved);
	rc.res.body = "";
	rc.res.body += "{\n";

	if (!matching_user) {
		this->users.push_back(recieved);
		rc.res.body += format_json_line("status", "created", true);
	}
	else if (matching_user->auth_key.size()) {
		rc.res.body += format_json_line("status", "already", true);
	}
	else if (recieved.password == matching_user->password){
		matching_user->auth_key = generate_cookie();
		rc.res.body += format_json_line("status", "ok", false);
		rc.res.body += format_json_line("auth", matching_user->auth_key, true);
	}
	else {
		rc.res.body += format_json_line("status", "wrong", true);
	}
	rc.res.body += "}";
}
