#include "HTTPConfig.hpp"

bool	HTTPConfig::parse_infile(std::ifstream &f, bool space_mode) {
	char                    buffer[BUFFER_SIZE];
	std::string				line;
	std::string				temp = "";
	std::streamsize         bytes;
	HTTPConfig::t_parser    opt;

	if (BUFFER_SIZE < 2) {
		std::cerr << "Buffer size too small (check HTTPConfig.hpp or use custom flag -D to define a new one)" << std::endl;
		return (1);
	}

	if (space_mode) { std::getline(f, line); } // getline dans le vide pour oublier premiere ligne
	opt.options = space_mode * O_SPACE_MODE;
	opt.line = 1;
	opt.in_http = false;
	opt.current_serv = &(this->default_config);
	bytes = BUFFER_SIZE - 1;
	do {
		if (opt.options & O_SPACE_MODE) {
			std::getline(f, line);
			opt.line++;
			if (f.eof()) { break ; }
			if (this->understand_the_line(temp + line, temp, opt) == 1)
				return (1);
		} else {
			f.read(buffer, BUFFER_SIZE - 1);
			bytes = f.gcount();
			buffer[bytes] = '\0';
			if (this->understand_the_line(temp + buffer, temp, opt) == 1)
				return (1);
		}
	} while (bytes == BUFFER_SIZE - 1);

	if (!opt.blocks.empty() && HTTPConfig::warning("Blocks not closed", 0, opt.options)) { return (1); }
	return (0);
}


// -1 = Continue
// 0 = OK
// 1 = Error parsing && ERROR_STOP on
int HTTPConfig::understand_the_line(std::string buffer, std::string & temp, HTTPConfig::t_parser &opt) {
	std::string				cmd;
	std::pair<char, int>    delim;
	std::string             cut;
	int						ret;

	temp = "";
	while (true) {
		delim = this->search_delim(buffer, opt);
		if (!delim.first && !(opt.options & O_SPACE_MODE)) {
			// if no delim, then we haven't read enough
			temp = buffer;
			return (-1);
		}
		else if (!delim.first) {
			// we in space mode
			delim.first = '\n';
			delim.second = buffer.size();
			cmd = buffer;
		}
		else
			cmd = buffer.substr(0, delim.second);
		cut = this->trim_buffer(cmd);
		// Empty line in space mode (ignored)
		if ((opt.options & O_SPACE_MODE) && delim.first != '}' && cut.empty())
			return (-1);

		// DELIM is end of block
		if (delim.first == '}') {
			if (opt.blocks.size() == 0 || HTTPConfig::error("Extra '}'", opt.line, opt.options)) { return (1); }
			//else if (cut != "" && HTTPConfig::error("Missing separator", opt.line, opt.options)) { return (1); }
			else {
				if (opt.blocks.top() == "server") {
					opt.current_serv = &(this->default_config);
				}
				opt.blocks.pop();
				if (opt.blocks.empty())
					opt.in_http = false;
			}
		}

		// DELIM is start of block
		else if (delim.first == '{') {
			ret = this->set_block(cut, opt);
			if (ret == 2) {
				this->skip_block(buffer, delim.second);
				continue ;
			}
			opt.blocks.push(cut);
		}

		// DELIM is ; or \n (SPACE_MODE)
		else if (!cut.empty()) { ret = understand_the_cut(cut, opt); }
		buffer = buffer.substr(delim.second + 1 - (delim.first == '\n'));
		if (ret == 1)
			return (ret);
	}
	return (ret);
}


int	HTTPConfig::understand_the_cut(std::string & cut, HTTPConfig::t_parser &opt) {
	if (cut[0] == '#')
		return (0);
	if (cut.substr(0, 6) == "DEFINE") {
		return (this->set_define(cut, opt));
	}
	else if (opt.blocks.size() != 0 && opt.blocks.top() == "types")
		return (this->set_type(cut, opt));
	else {
		if (!opt.in_http && HTTPConfig::warning("Not DEFINE not in a HTTP block", opt.line, opt.options)) { return (1); }
		return (this->set_other(cut, opt));
	}
	return (0);
}
