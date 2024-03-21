#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <map>
#include <vector>
#include <algorithm>
#include "utils.hpp"
#include "Session.hpp"

class Cookie
{
    public :
        Cookie();
        ~Cookie();
        void controlCookies(std::map<std::string, std::string> header, std::string URI);
        time_t      getCookieTime(int day, int hour, int min);
        std::string convertIntoRealTime(time_t rawtime);

        void    setCookieHeader(std::string rawCookie);
        void    parseURI(std::string URI);
        void    makeBody(std::string color, std::string size);

        std::string getresBody() const;
        std::string getresCookieHeaderString() const;
    private :
        Cookie(const Cookie &copy);
        Cookie operator=(const Cookie &copy);

        //add user's private information
        std::map<std::string, std::string>  _reqCookieHeader;
        std::map<std::string, std::string>  _resCookieHeader;
        std::map<std::string, std::string>  _queryString;
        bool                                _queryStringExistance;
        bool                                _cookieExistance;
        std::string _resCookieHeaderString;
        std::string _resBody;
};