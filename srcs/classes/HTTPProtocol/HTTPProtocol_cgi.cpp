#include "HTTPProtocol.hpp"

#define EXECVE_FAILURE 127

bool	HTTPProtocol::exec_cgi(std::string file, std::string *interpreter, t_response_creator &r) {
	int	pipefd[2];

	if (pipe(pipefd) == -1) { return (1); }

	pid_t	pid = fork();
	if (pid == -1) { return (1); }
	else if (pid == 0) {
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
		std::cerr << "START EXEC" << std::endl;
		execve(command[0], command, NULL);
		std::cerr << "EXEC FAILED" << std::endl;
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
		for (int i = 0; i < CGI_TO * 2; i++) {
			if (waitpid(pid, &status, WNOHANG) == pid) {
				timeout = false;
				break ;
			}
			usleep(500000);
		}
		if (timeout) {
			std::cout << "TIMEOUT" << std::endl;
			r.err_code = 500;
			kill(pid, SIGKILL);
			return (0);
		}
		std::cout << "EXIT = " << WIFEXITED(status) << " and SIGNAL = " << WIFSIGNALED(status) << std::endl;
		if (WIFEXITED(status) && WEXITSTATUS(status) == EXECVE_FAILURE) {
			std::cout << "execve FAILED" << std::endl;
			r.err_code = 500;
			return (0);
		}

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
