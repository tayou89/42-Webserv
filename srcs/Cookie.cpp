#include "../include/Cookie.hpp"

cookie::cookie()
{
    std::string serverName = "webserv"; //change this!!!
    
    this->setCookieFile(serverName + "_cookie.txt");
    int fd = open(this->getCookieFile().c_str(), O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
        ;
}

cookie::~cookie()
{

}

std::string cookie::getCookieFile() const
{
    return (this->cookieFile);
}

void        cookie::setCookieFile(std::string fileName)
{
    this->cookieFile = fileName;
}

void    cookie::eraseCookies()
{
    int fd;

    fd = open(this->getCookieFile().c_str(), O_RDWR);
    if (fd == -1)
        ;
    //gnl each line, and erase expired cookies
}

void    cookie::controlCookies(std::map<std::string, std::string> header)
{
    std::map<std::string, std::string>::iterator itr = header.find("Cookie");
    
    eraseCookies();
    if (itr == header.end())
    {
        //cookie header does not exist -> make new cookie, write it, return it
    }
    else
    {
        //what action should it make
    }
}

cookie::cookie(const cookie &copy)
{

}

cookie cookie::operator=(const cookie &copy)
{

}