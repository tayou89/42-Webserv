#include "../include/Cookie.hpp"

cookie::cookie()
{
    
}

cookie::~cookie()
{

}

cookie::cookie(const cookie &copy)
{

}

cookie cookie::operator=(const cookie &copy)
{

}

time_t cookie::getCookieTime(int day, int hour, int min)
{
    time_t  rawtime;
    time(&rawtime);

    rawtime = rawtime + (day * 24 * 60 * 60) + (hour * 60 * 60) + (min * 60);
    return (rawtime);
}

std::string cookie::convertIntoRealTime(time_t rawtime)
{
    struct tm   *timeinfo = gmtime(&rawtime);

    char buffer[80];
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    std::string cookieTime(buffer);
    return (cookieTime);
}

std::string cookie::makeNewCookie()
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

void    cookie::eraseCookies()
{
    std::vector<std::string> eraseList;

   for (std::map<std::string, time_t>::iterator itr = this->cookieList.begin(); itr != this->cookieList.end(); ++itr)
   {
        if (this->getCookieTime(0, 0, 0) > itr->second)
            eraseList.push_back(itr->first);
   }
   for (std::vector<std::string>::iterator itr = eraseList.begin(); itr != eraseList.end(); ++itr)
   {
        this->cookieList.erase(this->cookieList.find(*itr));
   }
}

std::string cookie::controlCookies(std::map<std::string, time_t> header)
{
    std::map<std::string, time_t>::iterator itr = header.find("Cookie");
    
    eraseCookies();
    if (itr == header.end())
    {
        //cookie header does not exist -> make new cookie, write it, return it
        std::string newCookieName = this->makeNewCookie();
        time_t      newCookieDate = getCookieTime(1, 0, 0);
        this->cookieList.insert(std::make_pair(newCookieName, newCookieDate));
        //return set-Cookie header field
        return ("SID=" + newCookieName + "; Expires=" + convertIntoRealTime(newCookieDate));
    }
    else
    {
        //what action should it make??
    }
}
