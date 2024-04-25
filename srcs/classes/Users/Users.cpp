#include "Users.hpp"

Users::Users(void) {}

void	Users::load_data(std::string const &path) {
	std::ifstream	user_file((path + ".usr").c_str());
	if (!user_file.good()) {
		return ;
	}

	std::string	data;
	t_user		entry;
	size_t		sep;
	for (short i = 0; i < 10 && !user_file.eof(); i++) {
		std::getline(user_file, data);
		sep = data.find(':');
		if (sep == std::string::npos) { continue ; }
		entry.name = data.substr(0, sep);
		entry.password = data.substr(sep + 1);
		this->users.push_back(entry);
		std::cout << entry.name << " " << entry.password << std::endl;
	}

	user_file.close();

	std::ifstream	session_file((path + ".session").c_str());
	if (!session_file.good()) {
		return ;
	}

	t_session	session;
	// Yeah thats a lot ig
	for (int i = 0; i < 2000000 && !session_file.eof(); i++) {
		std::getline(session_file, data);
		sep = data.find(':');
		if (sep == std::string::npos) { continue ; }
		session.auth = data.substr(0, sep);
		session.name = data.substr(sep + 1);
		std::cout << session.auth << "->" << session.name << std::endl;
		for (std::vector<t_user>::iterator it = this->users.begin();
				it != this->users.end(); it++) {
			if (it->name == session.name) {
				this->sessions.push_back(session);
				break ;
			}
		}
	}
}

void	Users::save_sessions(std::string const & path) const {
	std::ofstream	session_file((path + ".session").c_str());
	for (std::vector<t_session>::const_iterator it = this->sessions.begin();
			it != this->sessions.end(); it++) {
		session_file << it->auth << ":" << it->name << "\n";
	}
}

Users::~Users(void) {};
