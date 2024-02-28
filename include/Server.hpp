#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "Request.hpp"
#include "Response.hpp"

class Server {
    public :
        Server();
        ~Server();

        void    startServer(int fd);
        void    setEnvp(char **envp);
        request getRequest() const;

    private :
        Server(const Server&);
        Server& operator=(const Server&);
        // Protocol    _protocol;
        request     _request;
        response    _response;
        char        **envp;
};