#include "../include/ConfigUtil.hpp"
#include <cctype>
#include <dirent.h>
#include <map>
#include <sstream>
#include <unistd.h>
#include <vector>

int ConfigUtil::getStringCount(const std::string &string) {
  std::stringstream stringStream(string);
  std::string buffer;
  int stringCount = 0;

  while (stringStream >> buffer)
    ++stringCount;
  return (stringCount);
}

char ConfigUtil::getParameterTerminator(const std::string &directive) {
  if (isBlockDirective(directive) == true)
    return ('{');
  else
    return (';');
}

void ConfigUtil::removeComments(std::string &text) {
  std::stringstream stringStream(text);
  std::string nonCommentText;
  std::string nonCommentString;
  std::string commentString;

  while (std::getline(stringStream, nonCommentString, '#')) {
    nonCommentText += nonCommentString;
    std::getline(stringStream, commentString);
  }
  text = nonCommentText;
}

void ConfigUtil::freeStringArray(char **&stringArray) {
  std::size_t i;

  if (stringArray == NULL)
    return;
  for (i = 0; stringArray[i] != NULL; i++) {
    delete[] stringArray[i];
    stringArray[i] = NULL;
  }
  delete[] stringArray;
  stringArray = NULL;
}

bool ConfigUtil::isIPAddressFormat(const std::string &string) {
  std::vector<std::string> octetVector;
  std::string octet;
  int octetCount, i;

  octetVector = splitString(string, '.');
  octetCount = octetVector.size();
  if (octetCount != 4)
    return (false);
  for (i = 0; i < octetCount; i++) {
    octet = octetVector[i];
    if (isNumberString(octet) == false)
      return (false);
    if (isValidOctetNumber(std::atoi(octet.c_str())) == false)
      return (false);
  }
  return (true);
}

bool ConfigUtil::isValidOctetNumber(const int &number) {
  if (number >= 0 && number <= 255)
    return (true);
  else
    return (false);
}

bool ConfigUtil::isValidErrorStatusCode(const int &number) {
  if (number < 300 || number > 600)
    return (false);
  else
    return (true);
}

bool ConfigUtil::isValidPortNumber(const int &number) {
  if (number < 1 || number > 65535)
    return (false);
  else
    return (true);
}

bool ConfigUtil::isNumberString(const std::string &string) {
  int stringSize = string.size();
  int i;

  if (stringSize == 0)
    return (false);
  for (i = 0; i < stringSize; i++) {
    if (std::isdigit(string[i]) == false)
      return (false);
  }
  return (true);
}

bool ConfigUtil::isAlphabetNumberString(const std::string &string) {
  int stringSize = string.size();
  int i;

  if (stringSize == 0)
    return (false);
  for (i = 0; i < stringSize; i++) {
    if (std::isalnum(string[i]) == false)
      return (false);
  }
  return (true);
}

bool ConfigUtil::isConvertibleToSizeT(const std::string &string) {
  std::stringstream stringStream(string);
  std::size_t beforeSize, size;
  std::size_t unit = convertByteUnit(string[string.size() - 1]);

  stringStream >> size;
  if (stringStream.fail() == true)
    return (false);
  beforeSize = size;
  size = size * unit;
  if ((size / unit) != beforeSize)
    return (false);
  return (true);
}

bool ConfigUtil::isDelimeter(const char &character) {
  if (character == '{' || character == '}' || character == ';')
    return (true);
  else
    return (false);
}

bool ConfigUtil::isBlockDirective(const std::string &directive) {
  if (directive == SERVER_DIRECTIVE || directive == LOCATION_DIRECTIVE ||
      directive == TYPES_DIRECTIVE)
    return (true);
  else
    return (false);
}

bool ConfigUtil::isSizeString(const std::string &string) {
  std::string copyString = string;
  std::size_t stringSize = string.size();
  char unit;

  if (stringSize == 0)
    return (false);
  unit = string[stringSize - 1];
  if (isSizeUnit(unit) == true)
    copyString = string.substr(0, stringSize - 1);
  if (isNumberString(copyString) == false)
    return (false);
  return (true);
}

bool ConfigUtil::isSizeUnit(const char &character) {
  if (character == 'K' || character == 'M' || character == 'G')
    return (true);
  else
    return (false);
}

bool ConfigUtil::isHTTPMethod(const std::string &string) {
  std::string HTTPMethods[] = {"GET",     "POST",  "DELETE",  "PUT",  "HEAD",
                               "OPTIONS", "PATCH", "CONNECT", "TRACE"};
  std::string *start = HTTPMethods;
  std::string *end = HTTPMethods + (sizeof(HTTPMethods) / sizeof(std::string));

  if (std::find(start, end, string) != end)
    return (true);
  else
    return (false);
}

bool ConfigUtil::isExistingFile(const std::string &filePath) {
  if (access(filePath.c_str(), F_OK) == 0)
    return (true);
  else
    return (false);
}

bool ConfigUtil::isReadableFile(const std::string &filePath) {
  if (access(filePath.c_str(), R_OK) == 0)
    return (true);
  else
    return (false);
}

bool ConfigUtil::isExecutableFile(const std::string &filePath) {
  if (access(filePath.c_str(), X_OK) == 0)
    return (true);
  else
    return (false);
}

bool ConfigUtil::isWritableFile(const std::string &filePath) {
  if (access(filePath.c_str(), W_OK) == 0)
    return (true);
  else
    return (false);
}

bool ConfigUtil::isURIDelimeter(const char &character) {
  if (character == ':' || character == '/' || character == '?' ||
      character == '&' || character == '#')
    return (true);
  else
    return (false);
}

bool ConfigUtil::isFileExtension(const std::string &file,
                                 const std::string &extension) {
  std::size_t extensionStart = file.rfind(extension);

  if (extensionStart == std::string::npos)
    return (false);
  if (extensionStart == 0)
    return (false);
  if (file[extensionStart - 1] != '.')
    return (false);
  if (extensionStart + extension.size() != file.size())
    return (false);
  return (true);
}

std::size_t ConfigUtil::convertToSizeT(const std::string &string) {
  std::stringstream stringStream(string);
  std::size_t size;

  stringStream >> size;
  return (size);
}

std::size_t ConfigUtil::convertByteUnit(const char &byteUnit) {
  if (byteUnit == 'K')
    return (1024);
  else if (byteUnit == 'M')
    return (1024 * 1024);
  else if (byteUnit == 'G')
    return (1024 * 1024 * 1024);
  else
    return (1);
}

std::size_t ConfigUtil::findDelimeter(const std::string &string) {
  std::size_t stringSize = string.size();
  std::size_t i;

  for (i = 0; i < stringSize; i++) {
    if (isDelimeter(string[i]) == true)
      return (i);
  }
  return (std::string::npos);
}

std::size_t ConfigUtil::findURIDelimeter(const std::string &uri,
                                         const std::size_t &index) {
  std::size_t uriSize = uri.size();
  std::size_t i;

  for (i = index; i < uriSize; i++) {
    if (isURIDelimeter(uri[i]) == true)
      return (i);
  }
  return (std::string::npos);
}

std::vector<std::string> ConfigUtil::splitString(const std::string &string,
                                                 const char &delimeter) {
  std::vector<std::string> splitStrings;
  std::stringstream stringStream(string);
  std::string buffer;

  while (std::getline(stringStream, buffer, delimeter))
    splitStrings.push_back(buffer);
  return (splitStrings);
}

std::vector<std::string>
ConfigUtil::getStringVector(const std::string &string) {
  std::stringstream stringStream(string);
  std::vector<std::string> stringVector;
  std::string buffer;

  while (stringStream >> buffer)
    stringVector.push_back(buffer);
  return (stringVector);
}

std::string ConfigUtil::convertIntToString(const int &integer) {
  std::stringstream stringStream;
  std::string string;

  stringStream << integer;
  stringStream >> string;
  return (string);
}

bool ConfigUtil::isMimeTypeFormat(const std::string &string) {
  std::vector<std::string> strings = splitString(string, '/');
  std::size_t stringCount = strings.size();
  std::size_t i;

  if (stringCount != 2)
    return (false);
  for (i = 0; i < stringCount; i++) {
    if (strings[i] == "")
      return (false);
  }
  return (true);
}
