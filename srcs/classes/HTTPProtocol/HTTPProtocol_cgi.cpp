#include "HTTPProtocol.hpp"

// Useless define to specify it. A cgi shall not return 127 or it will be ignored
#define EXECVE_FAILURE 127

std::string	*HTTPProtocol::get_default_interpreter(std::string const & file_type) {
	if (file_type == "py") { return (new std::string(PY)); }
	return (NULL);
}

bool	HTTPProtocol::exec_cgi(std::string file, std::string *interpreter, t_response_creator &r) {
	int	pipefd[2];

	if (pipe(pipefd) == -1) { r.err_code = 500; return (0); }

	pid_t	pid = fork();
	if (pid == -1) { close(pipefd[0]); close(pipefd[1]); r.err_code = 500; return (0); }
	else if (pid == 0) {
		if (!interpreter && r.conf->default_interpreter) {
			interpreter = get_default_interpreter(r.file_type);
		}
		// CHILD PROCESS
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		char *command[4];
		if (interpreter) {
			command[0] = const_cast<char *>(interpreter->c_str());
			command[1] = const_cast<char *>(file.c_str());
			if (r.req.body.size() > 0) {
				command[2] = const_cast<char *>(r.req.body.c_str());
				command[3] = NULL;
			} else {
				command[2] = NULL;
			}
		} else {
			command[0] = const_cast<char *>(file.c_str());
			if (r.req.body.size() > 0) {
				command[1] = const_cast<char *>(r.req.body.c_str());
				command[2] = NULL;
			} else {
				command[1] = NULL;
			}
		}
		execve(command[0], command, NULL);
		exit(EXECVE_FAILURE);
	}
	else {
		// FATHER PROCESS
		close(pipefd[1]);
		std::string	ret = "";
		char		buffer[4096];
		ssize_t		bytes;

		bool	timeout = true;
		int	status;
		for (int i = 0; i < CGI_LOOP; i++) {
			if (waitpid(pid, &status, WNOHANG) == pid) {
				timeout = false;
				break ;
			}
			usleep(CGI_SLEEP);
		}
		if (timeout) {
			// timeout
			std::cout << "TIMEOUT" << std::endl;
			r.err_code = 504;
			kill(pid, SIGKILL);
			return (0);
		}
		if (WIFEXITED(status) && WEXITSTATUS(status) == EXECVE_FAILURE) {
			// execve fail
			r.err_code = 500;
			return (0);
		}

		// Read output
		while ((bytes = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes] = '\0';
			ret += buffer;
		}
		close(pipefd[0]);

		r.res.body = ret;
		r.req.body = ret;

		r.has_cgi = true;
		r.file = "";

		unsigned long	fword = 0;
		for (; fword < ret.size() && isspace(ret[fword]); fword++) {}
		if (ret.substr(fword, 15) == "<!DOCTYPE html>") { r.file_type = "html"; }
	}
	return (0);
}


void HTTPProtocol::cgi(t_response_creator &r) const {
	HTTPConfig::t_cgi const *cgi = &(r.location->cgi);
	std::map<std::string, std::string>::const_iterator int_iter = cgi->cgi_interpreter.find("." + r.file_type);
	if (int_iter != cgi->cgi_interpreter.end()) {
		std::string	interpreter = int_iter->second;
		if (exec_cgi(r.file, &interpreter, r) == 0) { return ; }
	}
	for (std::set<std::string>::const_iterator it = cgi->cgi_exec.begin(); it != cgi->cgi_exec.end(); it++) {
		if (is_wildcard_match(r.file, *it) && exec_cgi(r.file, NULL, r) == 0) { return ; }
	}
}
