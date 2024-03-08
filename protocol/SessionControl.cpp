#include "../include/SessionControl.hpp"

SessionControl::SessionControl()
{
    std::string sessionID = "defaultSession";
    std::string lang = "null";
    _sessionList.push_back(createSession(sessionID, lang));
}

SessionControl::SessionControl(const SessionControl &copy)
{

}

SessionControl SessionControl::operator=(const SessionControl &copy)
{

}

SessionControl::~SessionControl()
{

}

void    SessionControl::controlSession(std::map<std::string, std::string> header)
{
    std::map<std::string, std::string>::iterator itr = header.find("Cookie");

    if (itr == header.end())
    {

    }
    else
    {
        if ()
    }

    
}

Session createSession(std::string sessionID, std::string lang)
{
    Session newSession(sessionID, lang);
    return (newSession);
}

Session SessionControl::getSession(std::string sessionID)
{
    std::vector<Session>::iterator itr = _sessionList.begin();

    for (; itr != _sessionList.end(); ++itr)
    {
        if (itr->getSessionID() == sessionID)
            return (*itr);
    }
    return (*(_sessionList.begin()));
}

void SessionControl::deleteSession(std::string sessionID)
{
    std::vector<Session>::iterator itr = _sessionList.begin();

    for (; itr != _sessionList.end(); ++itr)
    {
        if (itr->getSessionID() == sessionID)
        {
            _sessionList.erase(itr);
            return ;
        }
    }
    return ;
}