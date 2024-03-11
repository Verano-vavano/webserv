#include "HTTPServ.hpp"

void    HTTPServ::understand_request(std::string &request, HTTPServ::t_request &treq) {
	std::string line;
	int         index = 0;
	int         sub_index = 0;
	int         sub_sub_index = 0;

	treq.bad_request = false;

	// REQUEST LINE
	index = request.find("\r\n");
	line = request.substr(0, index);

	sub_index = line.find(' '); // after method
	treq.method = line.substr(0, sub_index);
	sub_index++;
	sub_sub_index = line.find(' ', sub_index); // after either HTTP or uri
	if (sub_sub_index < index) {
		treq.uri = line.substr(sub_index, sub_sub_index - sub_index);
		sub_index = sub_sub_index + 1;
	}

	// HEADERS are defined by a name and a list of values
	sub_index = index + 2; // sub_index points to the start of the headers and will move line to line
	index = request.find("\r\n\r\n", sub_index) + 1; // index points to the end of the headers

	std::pair<std::string, std::vector<std::string> > new_el;
	while (sub_index < index) {
		sub_sub_index = request.find(':', sub_index);
		new_el.first = request.substr(sub_index, sub_sub_index - sub_index);
		// If \n in key, then bad request lol
		if (new_el.first.find("\n") != std::string::npos) {
			treq.bad_request = true;
			return ;
		}
		sub_sub_index++;
		sub_index = request.find("\r\n", sub_index);
		new_el.second = HTTPServ::split_header_val(request.substr(sub_sub_index, sub_index - sub_sub_index));
		sub_index += 2;
	}

	// BODY
	treq.body = request.substr(index);

	/* DEBUG
	 * std::cout << "METHOD = " << treq.method << std::endl;
	 * std::cout << "URI = " << treq.uri << std::endl;
	 * std::cout << "BODY = " << treq.body << std::endl;
	 */

	return ;
}


std::vector<std::string>    HTTPServ::split_header_val(std::string val) {
    std::vector<std::string>    ret;
    std::string                 sub;
    int start, end, next;

    start = val.find_first_not_of(' ');
    while (start < val.size()) {
        next = val.find(',', start);
        end = val.find_last_not_of(' ', next);
        ret.push_back(val.substr(start, end - start + (val[next] != ',')));
        if (next >= val.size())
            break ;
        start = val.find_first_not_of(' ', next + 1);
    }
    std::cout << std::endl;
    return (ret);
}
