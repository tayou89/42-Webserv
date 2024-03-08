#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "Session.hpp"

class SessionControl
{
    public :
        SessionControl();
        ~SessionControl();
        Session getSession(std::string sessionID);
        void    deleteSession(std::string sessionID);
        void    controlSession(std::map<std::string, std::string> header);
        Session createSession(std::string sessionID, std::string lang);
    private :
        SessionControl(const SessionControl &copy);
        SessionControl operator=(const SessionControl &copy);
        
        //add user's private information
        std::vector<Session> _sessionList;
};