#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "Protocol.hpp"

class Server {
    public :
        Server();
        ~Server();

        void	getRequest(int fd);
        void	checkValidity();
        void	executeMethod();
        void	sendResponse(int fd);

        void	setEnvp(char **envp);
        std::string	getPath(char **envp, std::string cmd);

        void    GET_HEAD();
        void    POST();
        void    DELETE();
        void    PUT();
        void    OPTIONS();
        void    TRACE();

    private :
        Server(const Server&);
        Server& operator=(const Server&);

        Protocol    _protocol;
        char        **envp;
};