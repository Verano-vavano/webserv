#include "HTTPProtocol.hpp"

int    HTTPProtocol::understand_request(t_request &req, std::string &s) {
    std::string line;
    int         index = 0;
    int         sub_index = 0;
    int         sub_sub_index = 0;

    // REQUEST LINE
    index = s.find("\r\n");
    line = s.substr(0, index);

    sub_index = line.find(' '); // after method
    req.method = line.substr(0, sub_index);
    sub_index++;
    sub_sub_index = line.find(' ', sub_index); // after either HTTP or uri
    if (sub_sub_index < index) {
        req.uri = line.substr(sub_index, sub_sub_index - sub_index);
        sub_index = sub_sub_index + 1;
    }
    else
        req.uri = "NONE";
    req.http_version = line.substr(sub_index, index - sub_index);

    // HEADERS are defined by a name and a list of values
    sub_index = index + 2; // sub_index points to the start of the headers and will move line to line
    index = s.find("\r\n\r\n", sub_index) + 1; // index points to the end of the headers

    std::pair<std::string, std::vector<std::string> > new_el;
    while (sub_index < index) {
        sub_sub_index = s.find(':', sub_index);
        new_el.first = s.substr(sub_index, sub_sub_index - sub_index);
        // If \n in key, then bad s lol
        if (new_el.first.find("\n") != std::string::npos) {
            return (400);
        }
        sub_sub_index++;
        sub_index = s.find("\r\n", sub_index);
        new_el.second = HTTPProtocol::split_header_val(s.substr(sub_sub_index, sub_index - sub_sub_index));
		req.headers.insert(new_el);
        sub_index += 2;
    }

    // BODY
    req.body = s.substr(index);

    /* DEBUG
     * std::cout << "METHOD = " << req.method << std::endl;
     * std::cout << "URI = " << req.uri << std::endl;
     * std::cout << "BODY = " << req.body << std::endl;
     */

    return (200);
}


std::vector<std::string>    HTTPProtocol::split_header_val(std::string val) {
    std::vector<std::string>    ret;
    std::string                 sub;
    unsigned int start, end, next;

    start = val.find_first_not_of(' ');
    while (start < val.size()) {
        next = val.find(',', start);
        end = val.find_first_not_of(' ', next);
        ret.push_back(val.substr(start, end - start));
        if (next >= val.size())
            break ;
        start = val.find_first_not_of(' ', next + 1);
    }
    return (ret);
}
