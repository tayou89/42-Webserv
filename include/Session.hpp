#pragma once

#include <iostream>
#include <map>

class Session
{
    public:
        Session(std::string sessionID);
        ~Session();
        Session(const Session& copy);
        Session operator=(const Session& copy);

        std::string getSessionID() const;
        std::map<std::string, std::string>  getSessionMap() const;
        void    addSessionMap(std::string key, std::string value);
    private:
        //personal information of user
        std::string _sessionID;
        std::map<std::string, std::string> _sessionMap;
};