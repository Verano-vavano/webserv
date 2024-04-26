# Miniweb!
We are Hugo, Thibault and Theo, our team name is **Über Frosch**

This is our fully configurable HTTP/1.1 server.

# Main functionalities
## Configuration
One of our points in honor for this server was for it to be as configurable as possible. The configuration file is very similar as Nginx's.<br>
The config file must be included as the first parameter of the executable, and should end with .conf .<br>
Every command must be separated by a separator (';' by default). Blocks are used to englobe commandswithin { and }.
### Defines
Defines are used to modify the way that the configuration file will be read. They must be outside of every block, in the general scope.
- 'DEFINE SPACE_MODE;': This mode is used so that '\n' is considered as a delimiter. The file will also be read line by line.
- 'DEFINE ERROR_STOP;': If an error occurs, the configuration file will stop from being read furthermore.
- 'DEFINE WARNING_AS_ERROR;': Every warnings will be considered as errors.
- 'DEFINE SILENT;': No errors, warnings or fatal errors will be displayed.
- 'DEFINE TOGGLE_BOOL;': Overwrites the standard boolean functionality by toggling it from on to off and from off to on.
### http block
The http block is used to contain every server related infos.<br>
Writing outside of the http block anything outside of a define will result in a warning, but it will be used in the default configuration.
### server block
The server blocks are used to configurate different servers.<br>
Every server are defined by a server name and a port. By default, a server is initialized as such : 'localhost:8080'. But they can be overwritten by specifying them next to the server clause : 'server www.my_server_name.com:9000 {}'.
#### location block
The location blocks are used to specify certain rules depending on the requested URI<br>
- `root` and `alias` will modify where to search for the URI. `root` will append itself, as alias will replace.
- `index`, if no file is specified, will be used instead. `index none` will reset the index to an empty string (no indexing, might be useful for directory listing for example)
- `methods` and `not_methods` can be used to specify or unspecify methods to a directory. (see methods)
- `func`, `post_func` and `del_func` can be used to specify the behaviour of certain methods. (see func)
- `log` and `unlog`: see log
- `dir_listing` can be used to enable or disable directory listing. It is off by default. It looks like the `autoindex` of Nginx.
- `cgi-exec` and `cgi-interpreter` are used to define cgi-behaviour. (see cgi)
#### types block
The `types` block is used to define types according to the file type. This type will be used for the `Content-type` header.<br>
Every line must be defined as such : `filetype type`.
#### Other useful commands
The following are all the commands that can be used in a server block :
- `listen`: changes the port listened by the server. Will not merge with any existing one (for that, specify the port next to `server`).
- `server_name`: changes the server_name. Same issue as `listen`.
- `chunked_transfer_encoding [on/off]`: disables or enables CTE (on by default). CTE sends the response by packages. Without it, it sends the whole response.
- `client_body_buffer_size`: defines by how much we read the body of the request every time.
- `client_header_buffer_size`: same but with headers.
- `client_max_body_size`: defines a maximum size for every request with a body. If the body is too large, the server will send a 413 error (Request Entity Too Large).
- `default_type`: defines a default type if the type has not been deduced by the server.
- `error_page (err_code [...] /uri_to_page [=ret])`: defines a standard uri to handle errors. '=ret' allows to change the status of the error afterwards.
- `keepalive_requests (num)`: close client's connection after num requests.
- `default_interpreter`: defines if the default interpreter should be used if none specified
- `add_header key value`: adds custom headers to the response.
<a/>

### Methods
`methods` will allow some methods inside a directory.<br>
`std` is a keyword representing `GET`, `POST` and `DELETE`.<br>
Adding a `/` in front of the method will disable it. The methods aren't case sensitive.<br>
`not_methods` does the opposite of `methods`. It will always disable the methods mentionned, except if a `/` is specified.
### Logs
`log` can be used to log requests. It is written as such : <br>
`log [all/server/client/errors/nothing/[err_codes]] [default/focused/independant =filename]`<br>
`err_codes` must be 3 characters long and represent an error code. Unspecified characters must be written with a lowercase `x` as such: `4xx` for every errors starting with a 4.<br>
`all` will englobe every log possibilities.<br>
`server` is `5xx`. `client` is `4xx`. `errors` is `4xx 5xx`.<br>
`log nothing` will remove all logs. It is similar to `unlog all` (see after).<br>
`default` will log in `./miniweb.log`. `focused` will log in `./miniweb.focused.log` and `independant` will log in the filename specified.<br>
The `unlog` command can be used to unlog stuff. If no filetype is specified, it will remove all. If no err_code is specified, it will remove `errors`.<br>
`log` by default uses `all` and `default`.
### Comments
You can simply add comments by adding a `#` at the beginning of the command line. Everything will be ignored until the first delimiter.
## Server mainloop
The server will first listen to all ports specified in the server blocks.<br>
The server will add clients as they come, and wait for them to send requests. When a request comes, the server will only read every so often to be sure that the client is still sending.<br>
When the request has been fully read, the server creates the response and sends the data.<br>
### Request getter
The server reads every loop if he has to. It will first read until he encounters a CRLFCRLF or until there is no data left. It implies either a bad request (hence error 400) or the end of the headers. It will use the headers content to get the Host and get the right configuration (if multiple hostnames for one port). If the payload is too large, the server will clear the socket and will return a 413.
### Response creator
To create the response, the server will first parse the URI to get the defined location (/ at minima) and get the correct file to read. It will execute CGIs if it is CGI-abled.<br>
It will then handle the method. For now, there are only 3: GET, POST and DELETE.<br>
If the method is not among the allowed methods defined by `methods` and `not_methods`, it will return a 405.
#### CGI
CGIs are executed if and only if :
- A `cgi-interpreter` has been defined for this specific type.
- A `cgi-exec` wildcard matches with the URI.
<a/>

If a timeout occurs, a 504 error is returned.
#### GET
GET will simply read the file or display the directory listing. If CTE is enabled, it will only check if the file is readable.
#### POST
POST behaviour depends on how it is defined by post_func in the config file.
- `client_manager`: will parse a strictly written json file, store the data in .usr and .session.
- `upload_manager`: default behaviour. Will upload the file in the request directory.
#### DELETE
DELETE behaviour works the same as POST. Both can be defined the same way by simply using `func` method. Else you can use `del_func`.
- `client_manager`: will unlog a client.
- `upload_manager`: will delete the file.
#### Type checker
The server will now check if the file complies with the Accept header from the request, and its associated q-value. Else it will return 406.
#### Headers
It now adds the headers. For now, it only means `Content-type`, `Content-length` or `Transfer-encoding: Chunked`, `Connection`, `Accept` if 405 and custom headers.
#### Sending the response
It sends the response either by straight up sending it, or by first sending the headers, then the body in equally sized packages.
## Close the server
The server has a SIGINT handler that allows the program to quit itself properly. The only way to gracefully quit the program now is by sending it a SIGINT (^C or via kill).
