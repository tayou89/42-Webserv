#ifndef CONFIG_UTIL_HPP
#define CONFIG_UTIL_HPP

#include <stack>
#include <string>

#define SERVER_DIRECTIVE "server"
#define LISTEN_DIRECTIVE "listen"
#define SERVER_NAME_DIRECTIVE "server_name"
#define ROOT_DIRECTIVE "root"
#define ERROR_PAGE_DIRECTIVE "error_page"
#define CLIENT_MAX_DIRECTIVE "client_max_body_size"
#define HEADER_MAX_DIRECTIVE "large_client_header_buffers"
#define LOCATION_DIRECTIVE "location"
#define INDEX_DIRECTIVE "index"
#define AUTOINDEX_DIRECTIVE "autoindex"
#define REDIRECTION_DIRECTIVE "return"
#define LIMIT_EXCEPT_DIRECTIVE "limit_except"
#define CGI_PASS_DIRECTIVE "cgi_pass"
#define INCLUDE_DIRECTIVE "include"
#define TYPES_DIRECTIVE "types"

class ConfigUtil {
public:
  static int getStringCount(const std::string &string);
  static char getParameterTerminator(const std::string &directive);
  static void removeComments(std::string &text);
  static void freeStringArray(char **&stringArray);
  static bool isHTTPMethod(const std::string &string);
  static bool isIPAddressFormat(const std::string &string);
  static bool isValidOctetNumber(const int &number);
  static bool isNumberString(const std::string &string);
  static bool isAlphabetNumberString(const std::string &string);
  static bool isValidErrorStatusCode(const int &number);
  static bool isValidPortNumber(const int &number);
  static bool isDelimeter(const char &character);
  static bool isBlockDirective(const std::string &directive);
  static bool isConvertibleToSizeT(const std::string &string);
  static bool isSizeString(const std::string &string);
  static bool isSizeUnit(const char &character);
  static bool isExistingFile(const std::string &filePath);
  static bool isReadableFile(const std::string &filePath);
  static bool isWritableFile(const std::string &filePath);
  static bool isExecutableFile(const std::string &filePath);
  static bool isURIDelimeter(const char &character);
  static bool isMimeTypeFormat(const std::string &string);
  static bool isFileExtension(const std::string &file,
                              const std::string &extension);
  static std::size_t convertToSizeT(const std::string &string);
  static std::size_t convertByteUnit(const char &byteUnit);
  static std::size_t findDelimeter(const std::string &string);
  static std::size_t findURIDelimeter(const std::string &uri,
                                      const std::size_t &index);
  static std::string convertIntToString(const int &integer);
  static std::vector<std::string> getStringVector(const std::string &string);
  static std::vector<std::string> splitString(const std::string &string,
                                              const char &delimeter);
};

#endif
