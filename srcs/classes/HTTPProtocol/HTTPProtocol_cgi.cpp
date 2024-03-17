#include "HTTPProtocol.hpp"

bool	HTTPProtocol::exec_cgi(std::string file, std::string *interpreter, t_response_creator &r) {
	int	pipefd[2];
	std::cout << "CGI = " << file << " and INTERPRETER = " << interpreter << std::endl;

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
		execve(command[0], command, NULL);
		return (1);
	}
	else {
		// FATHER PROCESS
		close(pipefd[1]);
		std::string	ret = "";
		char		buffer[4096];
		ssize_t		bytes;

		while ((bytes = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes] = '\0';
			ret += buffer;
		}
		close(pipefd[0]);

		int	status;
		waitpid(pid, &status, 0);

		r.res.body = ret;

		unsigned long	fword = 0;
		for (; fword < ret.size() && isspace(ret[fword]); fword++) {}
		if (ret.substr(fword, 15) == "<!DOCTYPE html>") { r.file_type = "html"; }
	}
	return (0);
}
