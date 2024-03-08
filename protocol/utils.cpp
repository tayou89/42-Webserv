#include "../include/utils.hpp"

std::string getCurrentHttpDate() {
  // Get the current time
  time_t rawtime;
  time(&rawtime);

  // Convert to a struct tm
  struct tm *timeinfo = gmtime(&rawtime);
  // std::cout << "tm_hour: " << timeinfo->tm_hour << std::endl;
  timeinfo->tm_hour = (timeinfo->tm_hour + 9) % 24; // UTC to KST

  // Format the time to HTTP date format
  char buffer[80];
  strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
  std::string str(buffer);
  str = "Date: " + str + "\r\n";
  return (str);
}

std::string splitBeforeColon(std::string sentence) {
  size_t split_index;
  size_t i = 1;
  std::string key;

  split_index = sentence.find(":", 0);
  if (split_index == std::string::npos)
    return ("");
  while (i < split_index) {
    if (sentence[split_index - i] == ' ')
      i++;
    else
      break;
  }
  key = sentence.substr(0, split_index - i + 1);
  return (key);
}

std::string splitAfterColon(std::string sentence) {
  size_t split_index;
  size_t i = 1;
  std::string value;

  split_index = sentence.find(":", 0);
  if (split_index == std::string::npos)
    return ("");
  while (i < sentence.size() - split_index) {
    if (sentence[split_index + i] == ' ')
      i++;
    else
      break;
  }
  value = sentence.substr(split_index + i);
  return (value);
}

std::vector<std::string> splitString(const std::string &input, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(input);

  while (std::getline(tokenStream, token, delimiter)) {
    if (!token.empty())
      tokens.push_back(token);
  }

  return (tokens);
}
