#pragma once

#include <iostream>
#include "Protocol.hpp"

class TestServer {
    public :
        TestServer();
        ~TestServer();

        void getRequest(int fd);
        void checkValidity();
        void executeMethod();
        void sendResponse(int fd);

    private :
        TestServer(const TestServer&);
        TestServer& operator=(const TestServer&);

        Protocol _protocol;
};