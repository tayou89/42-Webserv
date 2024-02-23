#include "../include/Protocol.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "../include/TestServer.hpp"

int main(int argc, char **argv, char **envp)
{
    Protocol protocol;
    TestServer testserver;
    int fd;

    (void)argc;
    (void)argv;
    fd = open("test.txt", O_RDONLY);
    if (fd == -1)
    {
        std::cout << "open error" << std::endl;
        return 1;
    }
    try 
    {
        testserver.setEnvp(envp);
        testserver.getRequest(fd);
        testserver.checkValidity();
        testserver.executeMethod();
        testserver.sendResponse(fd);
    }
    catch (std::string &e)
    {
        if (e == "")
            std::cout << "error other than errorResponses" << std::endl;
        else
            std::cout << e << std::endl; //e is a error string, so just put it in the packet and send it
    }
    return 0;
}