#include "../include/utils.hpp"

std::string getCurrentHttpDate() {
  // Get the current time
  time_t rawtime;
  time(&rawtime);

  // Convert to a struct tm
  struct tm *timeinfo = gmtime(&rawtime);
  timeinfo->tm_hour = (timeinfo->tm_hour + 9) % 24; // UTC to KST

  // Format the time to HTTP date format
  char buffer[80];
  strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
  std::string str(buffer);
  str = "Date: " + str + "\r\n";
  return (str);
}

std::string getCurrentHttpDateForCookie() {
  // Get the current time
  time_t rawtime;
  time(&rawtime);

  // Convert to a struct tm
  struct tm *timeinfo = gmtime(&rawtime);
  timeinfo->tm_hour = (timeinfo->tm_hour + 9) % 24; // UTC to KST

  // Format the time to HTTP date format
  char buffer[80];
  strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
  std::string str(buffer);
  str = "Date: " + str;
  return (str);
}

std::string splitBefore(std::string sentence, std::string splitWord) {
  std::size_t split_index;
  std::size_t i = 1;
  std::string key;

  split_index = sentence.find(splitWord, 0);
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

std::string splitAfter(std::string sentence, std::string splitWord) {
  std::size_t split_index;
  std::size_t i = 0;
  std::string value;

  split_index = sentence.find(splitWord, 0);
  if (split_index == std::string::npos)
    return ("");
  while (i < sentence.size() - split_index) {
    if (sentence[split_index + 1 + i] == ' ')
      i++;
    else
      break;
  }
  value = sentence.substr(split_index + i + splitWord.size());
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

struct eventStatus makeStatus(int status, int ident) {
  struct eventStatus res;

  res.status = status;
  res.ident = ident;
  res.sub = -1;
  return (res);
}

struct eventStatus makeStatus(int status, int ident, int sub) {
  struct eventStatus res;

  res.status = status;
  res.ident = ident;
  res.sub = sub;
  return (res);
}

std::string getModifiedTime(time_t mTime) {
  struct tm *timeinfo = gmtime(&mTime);
  timeinfo->tm_hour = (timeinfo->tm_hour + 9) % 24;

  char buffer[80];

  strftime(buffer, 80, "%d-%b-%Y %H:%M", timeinfo);
  std::string MTstring(buffer);
  return (MTstring);
}

std::string intToString(int value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

std::vector<std::string> rgbToDecimal(const std::string &rgbHex) {
  std::vector<std::string> decimalValues;

  // Convert hexadecimal to decimal for each component
  for (int i = 0; i < 3; ++i) {
    int value;
    std::stringstream ss;
    ss << std::hex << rgbHex.substr(i * 2, 2);
    ss >> value;
    decimalValues.push_back(intToString(value));
  }
  return (decimalValues);
}

std::string decimalToRgb(int red, int green, int blue) {
  std::ostringstream oss;

  // Convert decimal to hexadecimal for each component
  char buffer[3];
  sprintf(buffer, "%02x", red);
  oss << buffer;
  sprintf(buffer, "%02x", green);
  oss << buffer;
  sprintf(buffer, "%02x", blue);
  oss << buffer;

  return oss.str();
}

void replace(std::string &str, const std::string &target,
             const std::string &replacement) {
  std::size_t pos = 0;

  while ((pos = str.find(target, pos)) != std::string::npos) {
    str.replace(pos, target.length(), replacement);
    pos += replacement.length(); // 다음 탐색 위치 설정
  }
}

std::vector<unsigned char>::iterator
findHeader(std::vector<unsigned char> &vec) {
  std::vector<unsigned char> needle;
  std::vector<unsigned char>::iterator iter;

  needle.push_back(13);
  needle.push_back(10);
  needle.push_back(13);
  needle.push_back(10);

  iter = std::search(vec.begin(), vec.end(), needle.begin(), needle.end());

  return (iter);
}

std::vector<unsigned char>::iterator
findNewLine(std::vector<unsigned char> &vec) {
  std::vector<unsigned char> needle;
  std::vector<unsigned char>::iterator iter;

  needle.push_back(13);
  needle.push_back(10);

  iter = std::search(vec.begin(), vec.end(), needle.begin(), needle.end());

  return (iter);
}

std::vector<unsigned char>::iterator
findNewLineNewLine(std::vector<unsigned char> &vec) {
  std::vector<unsigned char> needle;
  std::vector<unsigned char>::iterator iter;

  needle.push_back(10);
  needle.push_back(10);

  iter = std::search(vec.begin(), vec.end(), needle.begin(), needle.end());

  return (iter);
}

std::string makeTempFile() {
  char random[16];
  int devRandom;
  int idx = 0;

  devRandom = open("/dev/urandom", O_RDONLY);
  if (read(devRandom, random, 15) == -1)
    ;
  random[15] = '\0';
  close(devRandom);
  while (idx < 15) {
    if (random[idx] < 0)
      random[idx] *= -1;
    random[idx] = random[idx] % 25 + 'A';
    idx++;
  }
  std::string randomFileName(random);
  return (randomFileName);
}
