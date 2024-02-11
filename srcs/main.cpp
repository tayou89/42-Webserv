#include "../include/Protocol.hpp"

int main(void)
{
    Protocol protocol;
    // std::string startLine = "HEAD www.naver.com HTTP/1.1";
    // std::cout << protocol.readStartLine(startLine, 1000) << std::endl;
    std::string originalString = "Host: localhost:8080\r\nConnection: keep-alive\r\nsec-ch-ua: \"Chromium\";v=\"116\", \"Not)A;Brand\";v=\"24\", \"Google Chrome\";v=\"116\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1";
    std::cout << protocol.readHeader(originalString) << std::endl;
    return 0;
}