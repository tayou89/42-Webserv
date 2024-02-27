#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <map>

class cookie
{
    public :
        cookie();
        ~cookie();
        void        setCookieFile(std::string fileName);
        void        controlCookies(std::map<std::string, std::string> header);
        void        eraseCookies();
        std::string getCookieFile() const;

    private :
        cookie(const cookie &copy);
        cookie operator=(const cookie &copy);
        std::string cookieFile;
};