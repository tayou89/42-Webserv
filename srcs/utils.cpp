#include "../include/utils.hpp"

std::string getCurrentHttpDate() 
{
    // Get the current time
    time_t rawtime;
    time(&rawtime);

    // Convert to a struct tm
    struct tm* timeinfo = gmtime(&rawtime);
	// std::cout << "tm_hour: " << timeinfo->tm_hour << std::endl;
	timeinfo->tm_hour = (timeinfo->tm_hour + 9) % 24; //UTC to KST

    // Format the time to HTTP date format
    char buffer[80];
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	std::string str(buffer);
	str = "Date: " + str + "\r\n";
    return (str);
}