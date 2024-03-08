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
        std::string controlCookies(std::map<std::string, std::string> header);
        time_t      getCookieTime(int day, int hour, int min);
        std::string convertIntoRealTime(time_t rawtime);
        
        std::string makeNewSID();
        Session getSession(std::string sessionID);
        void    deleteSession(std::string sessionID);
        void    controlSession(std::map<std::string, std::string> header);
        Session createSession(std::string sessionID, std::string lang);
        void    setCookieHeader(std::string rawCookie);
    private :
        Cookie(const Cookie &copy);
        Cookie operator=(const Cookie &copy);

        //add user's private information
        std::vector<Session> _sessionList; //첫번째 노드는 비어있음
        std::map<std::string, std::string>  _reqCookieHeader;
        std::map<std::string, std::string>  _resCookieHeader;
};