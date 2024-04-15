#include "Users.hpp"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdlib.h>
#include <time.h>

Users::Users(void) {
	std::ifstream	user_file(".usr");
	if (!user_file.good()) {
		return ;
	}

	std::string	data;
	t_user		entry;
	size_t		sep;
	for (short i = 0; i < 10 && !user_file.eof(); i++) {
		std::getline(user_file, data);
		if (data.size() < 2) { continue ; }
		sep = data.find(':');
		if (sep == std::string::npos) { continue ; }
		entry.name = data.substr(0, sep);
		entry.password = data.substr(sep + 1);
		this->users.push_back(entry);
		std::cout << entry.name << " " << entry.password << std::endl;
	}
}

Users::~Users(void) {};

Users::t_user Users::parse_json(int &err_code, std::string json) {
	t_user received_user;

	size_t	user = json.find("\"user\"");
	size_t	pswd = json.find("\"password\"");
	if (user == std::string::npos || pswd == std::string::npos) {
		err_code = 400;
		return (received_user);
	}

	user += 6; // "user"
	pswd += 10; // "password"

	for (; user < json.length() && std::isspace(json[user]); user++);
	if (user < json.length() && json[user] == ':')
		user = json.find('"', user);
	for (; pswd < json.length() && std::isspace(json[pswd]); pswd++);
	if (pswd < json.length() && json[pswd] == ':')
		pswd = json.find('"', pswd);

	if (user >= json.length() || user == std::string::npos ||
			pswd >= json.length() || pswd == std::string::npos) {
		err_code = 400;
		return (received_user);
	}

	user++;
	pswd++;

	size_t	user_end = json.find('"', user);
	size_t	pswd_end = json.find('"', pswd);
	if (user_end == std::string::npos || pswd_end == std::string::npos) {
		err_code = 400;
		return (received_user);
	}

	received_user.name = json.substr(user, user_end - user);
	received_user.password = json.substr(pswd, pswd_end - pswd);
	std::cout << "- " << received_user.name << " : " << received_user.password << std::endl;
	return (received_user);
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

void Users::delete_user_auth(std::string body_json) {
	int mid = body_json.find(':');
	std::string auth = body_json.substr(mid, body_json.size() - mid);
	auth = auth.substr(2, auth.size() - 4);
	std::vector<t_user>::iterator users_it = this->users.begin();
	for(; users_it != this->users.end(); users_it++) {
		if (users_it->auth_key == auth)
			users_it->auth_key = "";
	}
}

bool is_delete_request(std::string body_json) {
	if (body_json.size() < 3) { return (false); }
	ulong size = body_json.find("\":") - 2;
	std::string mystr =  body_json.substr(2, size);
	std::cout << "IT IS " << mystr << std::endl;
	return (mystr == "delete");
}

void	Users::create_new_user(t_response_creator &rc, t_user& user) {
	std::ofstream	outfile;
	outfile.open(".usr", std::ios_base::app);
	if (outfile.good())
		outfile << user.name << ":" << user.password << "\n";
	this->users.push_back(user);
	rc.res.body += format_json_line("status", "created", true);
}

void Users::handle_user(t_response_creator &rc) { //HERE
	if (is_delete_request(rc.req.body)) { //remove this once the DELETE metode is implemented
		delete_user_auth(rc.req.body);
		rc.res.body = "";
		return;
	}
	t_user recieved = this->parse_json(rc.err_code, trim_body(rc.req.body));
	if (rc.err_code == 400) { return ; }
	t_user *matching_user = find_matching_user(recieved);
	rc.res.body = "";
	rc.res.body += "{\n";

	if (!matching_user) {
		std::cout << "Created"<< std::endl;
		this->create_new_user(rc, recieved);
	}
	else if (matching_user->auth_key.size()) {
		std::cout << "Idk" << std::endl;
		rc.res.body += format_json_line("status", "already", true);
	}
	else if (recieved.password == matching_user->password){
		std::cout << "Good" << std::endl;
		matching_user->auth_key = generate_cookie();
		rc.res.headers += "Set-Cookie: session_id=" + matching_user->auth_key + "; Secure; SameSite=None" + std::string(CRLF);
		rc.res.body += format_json_line("status", "ok", false);
		rc.res.body += format_json_line("auth", matching_user->auth_key, true);
	}
	else {
		std::cout << "Bad" << std::endl;
		rc.res.body += format_json_line("status", "wrong", true);
	}
	rc.res.body += "}";
	rc.is_json = true;
}
