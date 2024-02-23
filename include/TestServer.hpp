#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "Protocol.hpp"

class TestServer {
    public :
        TestServer();
        ~TestServer();

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
        TestServer(const TestServer&);
        TestServer& operator=(const TestServer&);

        Protocol    _protocol;
        char        **envp;
};