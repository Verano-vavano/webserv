#include "HTTPProtocol.hpp"

// Useless define to specify it. A cgi shall not return 127 or it will be ignored
#define EXECVE_FAILURE 127

std::string	*HTTPProtocol::get_default_interpreter(std::string const & file_type) {
	if (file_type == "py") { return (new std::string(PY)); }
	return (NULL);
}

bool	HTTPProtocol::exec_cgi(std::string file, std::string *interpreter, t_response_creator &r) {
	int	pipefd[2];

	if (pipe(pipefd) == -1) { return (1); }

	pid_t	pid = fork();
	if (pid == -1) { return (1); }
	else if (pid == 0) {
		if (!interpreter && r.conf->default_interpreter) {
			interpreter = get_default_interpreter(r.file_type);
		}
		// CHILD PROCESS
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		char *command[3];
		if (interpreter) {
			command[0] = const_cast<char *>(interpreter->c_str());
			command[1] = const_cast<char *>(file.c_str());
			command[2] = NULL;
		} else {
			command[0] = const_cast<char *>(file.c_str());
			command[1] = NULL;
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
			r.err_code = 500;
			kill(pid, SIGKILL);
			return (0);
		}
		if (WIFEXITED(status) && WEXITSTATUS(status) == EXECVE_FAILURE) {
			// execve fail
			return (1);
		}

		// Read output
		while ((bytes = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes] = '\0';
			ret += buffer;
		}
		close(pipefd[0]);

		r.res.body = ret;

		unsigned long	fword = 0;
		for (; fword < ret.size() && isspace(ret[fword]); fword++) {}
		if (ret.substr(fword, 15) == "<!DOCTYPE html>") { r.file_type = "html"; }
	}
	return (0);
}
