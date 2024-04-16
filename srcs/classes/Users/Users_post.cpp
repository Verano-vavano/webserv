#include "Users.hpp"

Users::t_user Users::parse_json_post(int &err_code, std::string json) {
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

void	Users::create_new_user(t_response_creator &rc, t_user& user) {
	std::ofstream	outfile;
	outfile.open(".usr", std::ios_base::app);
	if (outfile.good())
		outfile << user.name << ":" << user.password << "\n";
	this->users.push_back(user);
	rc.res.body += format_json_line("status", "created", true);
}

void	Users::add_session(t_session &session) {
	std::ofstream	outfile;
	outfile.open(".session", std::ios_base::app);
	if (outfile.good())
		outfile << session.auth << ":" << session.name << "\n";
}

void Users::handle_post(t_response_creator &rc) {
	t_user recieved = this->parse_json_post(rc.err_code, trim_body(rc.req.body));
	if (rc.err_code == 400) { return ; }
	t_user *matching_user = find_matching_user(recieved);
	rc.res.body = "";
	rc.res.body += "{\n";

	if (!matching_user) {
		std::cout << "Created"<< std::endl;
		this->create_new_user(rc, recieved);
	}
	/*else if (matching_user->auth_key.size()) {
		rc.res.body += format_json_line("status", "already", true);
	}*/
	else if (recieved.password == matching_user->password){
		std::cout << "Good" << std::endl;
		t_session	new_session;
		new_session.auth = generate_cookie();
		new_session.name = matching_user->name;
		rc.res.headers += "Set-Cookie: session_id=" + new_session.auth + "; Secure; SameSite=Strict" + std::string(CRLF);
		this->add_session(new_session);
		this->sessions.push_back(new_session);
		rc.res.body += format_json_line("status", "ok", false);
		rc.res.body += format_json_line("auth", new_session.auth, true);
	}
	else {
		std::cout << "Bad" << std::endl;
		rc.res.body += format_json_line("status", "wrong", true);
	}
	rc.res.body += "}";
	rc.is_json = true;
}
