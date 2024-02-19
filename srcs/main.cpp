#include "../include/Protocol.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "../include/TestServer.hpp"

int main(void)
{
    Protocol protocol;
    TestServer testserver;
    int fd;

    fd = open("test.txt", O_RDONLY);
    if (fd == -1)
    {
        std::cout << "open error" << std::endl;
        return 1;
    }
    testserver.getRequest(fd);
    testserver.checkValidity();
    testserver.executeMethod();
    testserver.sendResponse(fd);
    return 0;
}