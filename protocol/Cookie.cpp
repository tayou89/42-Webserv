#include "../include/Cookie.hpp"

Cookie::Cookie()
{
    std::string sessionID = "defaultSession";
    std::string lang = "null";
    _sessionList.push_back(createSession(sessionID, lang));
}

Cookie::~Cookie()
{

}

Cookie::Cookie(const Cookie &copy)
{

}

Cookie Cookie::operator=(const Cookie &copy)
{

}

time_t Cookie::getCookieTime(int day, int hour, int min)
{
    time_t  rawtime;
    time(&rawtime);

    rawtime = rawtime + (day * 24 * 60 * 60) + (hour * 60 * 60) + (min * 60);
    return (rawtime);
}

std::string Cookie::convertIntoRealTime(time_t rawtime)
{
    struct tm   *timeinfo = gmtime(&rawtime);

    char buffer[80];
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    std::string cookieTime(buffer);
    return (cookieTime);
}

std::string Cookie::makeNewSID()
{
    char	random_name[8];
	int		random_fd;
	int		i;

	i = 0;
	random_fd = open("/dev/urandom", O_RDONLY);
	if (read(random_fd, random_name, 8) == -1)
		;
	close(random_fd);
	while (8 > i)
	{
		if (random_name[i] < 0)
			random_name[i] = random_name[i] * -1;
		random_name[i] = random_name[i] % 93 + '!';
		i++;
	}
    std::string cookieName(random_name);
    return (cookieName);
}

std::string Cookie::controlCookies(std::map<std::string, std::string> header)
{
    std::map<std::string, std::string>::iterator itr = header.find("Cookie");
    
    if (itr == header.end())
    {
        //cookie header does not exist -> make new cookie, write it, return it
        std::string newSID = this->makeNewSID();
        //return set-Cookie header field with private information?
        return ("SID=" + newSID);
    }
    else
    {
        std::string sid = "";
        //read the cookie header, then make user specified response with that information
        setCookieHeader(itr->second);
        std::map<std::string, std::string>::iterator itr = _reqCookieHeader.begin();
        for (; itr != _reqCookieHeader.end(); ++itr)
        {
            if (itr->first == "SID")
                sid = itr->second;
            else
                _resCookieHeader.insert(std::make_pair(itr->first, itr->second));
        }
        if (sid != "")
        {
            std::map<std::string, std::string> ses = getSession(sid).getSessionMap();
            std::map<std::string, std::string>::iterator sesItr = ses.begin();
            for (; itr != ses.end(); ++itr)
                _resCookieHeader[itr->first] = itr->second; //중복될 경우 덮어씌워야해서 []
        }
        //go through _resCookieHeader to make user friendly response
    }
}

void    Cookie::setCookieHeader(std::string rawCookie)
{
    size_t index = 0;
    size_t pos = 0;
    std::string tmp;

    while (1)
    {
        index = rawCookie.find(";", pos);
        if (index == std::string::npos)
            break;
        tmp = rawCookie.substr(pos, index - pos);
        std::string key = splitBefore(tmp, "=");
        std::string value = splitAfter(tmp, "=");
        this->_reqCookieHeader.insert(std::make_pair(key, value));
        if (rawCookie.find(";", index + 1) == index + 1)
            break;
        pos = index + 1;
    }
}

Session Cookie::getSession(std::string sessionID)
{
    std::vector<Session>::iterator itr = _sessionList.begin();

    for (; itr != _sessionList.end(); ++itr)
    {
        if (itr->getSessionID() == sessionID)
            return (*itr);
    }
    return (*(_sessionList.begin()));
}

void    Cookie::deleteSession(std::string sessionID)
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

void    Cookie::controlSession(std::map<std::string, std::string> header)
{

}

Session Cookie::createSession(std::string sessionID, std::string lang)
{
    //or heap allocation?
    Session newSession(sessionID);
    return (newSession);
}
