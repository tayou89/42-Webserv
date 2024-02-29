#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <map>
#include <vector>
#include "utils.hpp"

class cookie
{
    public :
        cookie();
        ~cookie();
        std::string controlCookies(std::map<std::string, std::string> header);
        void        eraseCookies();
        std::string getCookieTime(int day, int hour, int min);
        std::string makeNewCookie();

    private :
        cookie(const cookie &copy);
        cookie operator=(const cookie &copy);
        std::map<std::string, std::string> cookieList;
};