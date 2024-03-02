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
        std::string controlCookies(std::map<std::string, time_t> header);
        void        eraseCookies();
        time_t      getCookieTime(int day, int hour, int min);
        std::string convertIntoRealTime(time_t rawtime);
        std::string makeNewCookie();

    private :
        cookie(const cookie &copy);
        cookie operator=(const cookie &copy);
        std::map<std::string, time_t> cookieList;
};