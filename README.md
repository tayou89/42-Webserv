# Webserv

## Introduction
Webserv is a custom HTTP server implementation in C++98. This project aims to create a fully functional web server capable of handling HTTP requests, serving static content, processing CGI, and more.

## Features
- HTTP/1.1 compliant
- Configurable via configuration file
- Non-blocking I/O with poll() (or equivalent)
- Support for GET, POST, and DELETE methods
- Static file serving
- CGI execution (Python and Perl)
- File uploads
- Multiple port listening
- Custom error pages
- Client body size limit
- Directory listing (configurable)
- URL redirection
- Cookie and session management

## Project Structure
```
.
├── Makefile
├── cgi/
├── config/
│   └── default.conf
├── document/
│   ├── cgi/
│   ├── css/
│   ├── html/
│   ├── javascript/
│   └── uploaded/
├── err_pages/
├── include/
├── main.cpp
├── mime/
├── parse/
├── protocol/
├── server/
└── subject/
```

- `cgi/`: Contains CGI-related implementation
- `config/`: Configuration files, including `default.conf`
- `document/`: Test files and resources for the server
- `err_pages/`: Custom error page templates
- `include/`: Header files
- `mime/`: MIME type definitions and handling
- `parse/`: Configuration parsing implementation
- `protocol/`: HTTP protocol-related implementations
- `server/`: Core server implementation

## Requirements
- C++98 compatible compiler
- Make

## Building the Project
To build the project, run:

```
make
```

This will compile the source files and generate the `webserv` executable.

## Running the Server
To run the server with the default configuration file:

```
./webserv
```

To run the server with a custom configuration file:

```
./webserv [path_to_config_file]
```

For example, to use the configuration file in the `config` folder:

```
./webserv config/default.conf
```

## Accessing the Test Page
After starting the server using the configuration file in the `config` folder, you can access the test page by opening a web browser and navigating to the URL specified in the configuration file.

For example, if your `default.conf` file contains:

```
server {
    listen      127.0.0.1:8082;
    ...
}
```

You can access the test page by entering the following URL in your web browser:

```
http://127.0.0.1:8082
```
<img width="1119" alt="Screen Shot 2024-08-28 at 9 27 58 PM" src="https://github.com/user-attachments/assets/73a267f8-7237-473c-80af-b0dbf033f3d7">

You should see a welcome page with the text "Welcome to Webserv Test" and a "Start Test" link. This page allows you to begin testing various features of your Webserv implementation.

## Testing the Server
The `document/` directory contains various files to test different features of the server. Here's how you can test some key features:

1. **Static File Serving**: 
   - Access `http://localhost:8082/` to view the index page.
   - Try `http://localhost:8082/test` to access a specific location.

2. **Directory Listing**: 
   - Visit `http://localhost:8082/autoindex` to see directory listing in action.

3. **CGI Execution**:
   - Python CGI: Access any `.py` file under `http://localhost:8082/cgi/`
   - Perl CGI: Access any `.pl` file under `http://localhost:8082/cgi/`

4. **File Upload**:
   - Go to `http://localhost:8082/upload` to test file upload functionality.
   - Uploaded files can be viewed at `http://localhost:8082/uploaded`

5. **File Deletion**:
   - Visit `http://localhost:8082/delete` to test file deletion.

6. **Redirection**:
   - Try accessing `http://localhost:8082/redirection` to test URL redirection.

7. **Cookie Handling**:
   - Access `http://localhost:8082/cookie` to test cookie functionality.

8. **Error Pages**:
   - Try accessing non-existent pages or simulating errors to see custom error pages.

## Configuration
The server uses a configuration file similar to NGINX. The default configuration file is located at `config/default.conf`. Here are some key configuration options:

- `listen`: Specifies the IP and port to listen on
- `server_name`: Sets the server name
- `root`: Defines the root directory for serving files
- `index`: Specifies default index files
- `error_page`: Sets custom error pages
- `client_max_body_size`: Limits the size of client request bodies
- `location`: Defines behavior for specific URL paths
- `limit_except`: Restricts HTTP methods for a location
- `autoindex`: Enables/disables directory listing
- `return`: Sets up redirections
- `cgi_pass`: Enables CGI processing for specific file extensions

For a complete example, refer to the `config/default.conf` file in the project.

## Notes
- The server uses non-blocking I/O and is designed to be efficient and resilient.
- All file descriptors are non-blocking, and operations are managed through poll() (or equivalent).
- The project follows C++98 standards and does not use any external libraries beyond the standard library.
- Make sure to check the configuration file for the correct port number when accessing the test page.