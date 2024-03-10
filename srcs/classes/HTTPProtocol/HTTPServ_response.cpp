#include "HTTPServ.hpp"

std::string	HTTPServ::create_response(std::string *request) {
    HTTPServ::t_request     req;
    this->understand_request(*request, req);
    HTTPServ::t_response    response;

    response.status_line = "HTTP/1.1";
    //this->handle_method(response, req);
    if (req.uri != "NONE")
        response.body = this->read_file(this->files_path + req.uri);
    if (response.body.err == 404) {
        response.body.buffer = "Error 404";
        response.status_line += " 404 Not Found";
    }
    else
        response.status_line += " 200 OK";
    if (req.uri.substr(req.uri.size() - 3) == ".js")
        response.headers += "Content-Type: application/javascript\r\n";
    else if (req.uri.substr(req.uri.size() - 5) == ".html")
        response.headers += "Content-Type: text/html\r\n";
    if (req.uri.substr(req.uri.size() - 4) == ".css")
        response.headers += "Content-Type: text/css\r\n";
    response.headers += "Content-Length:" + std::to_string(response.body.buffer.size()) + "\r\n";
    response.headers += "Connection: keep-alive\r\n";


    return (response.status_line + "\r\n" + response.headers + "\r\n" + response.body.buffer);
}
/*
int HTTPServ::handle_method(HTTPServ::t_response &resp, HTTPServ::t_request &req) {
    if (req.method == "GET")
        return (this->handle_get(resp, req));
    else
        return (500);
}*/

void    HTTPServ::set_headers(HTTPServ::t_response &resp, HTTPServ::t_request &req) {
    return ;
}
