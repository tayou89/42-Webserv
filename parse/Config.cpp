#include "../include/Config.hpp"

#include <cctype>
#include <cstdlib>
#include <iostream>

Config::Config(void) {}

Config::~Config(void) {}

Config::Config(const Config &object) { *this = object; }

Config &Config::operator=(const Config &object) {
  if (this == &object)
    return (*this);
  Location::operator=(object);
  _serverName = object._serverName;
  _ipAddress = object._ipAddress;
  _portNumber = object._portNumber;
  _locations = object._locations;
  return (*this);
}

Config::Config(const std::string &serverBlock)
    : Location(serverBlock, _getDirectiveSet()), _portNumber(0) {
  _setFunctionPTRSet();
  _setConfigData();
  _checkConfigValidation();
  _setLocation();
}

const std::string &Config::getServerName(void) const { return (_serverName); }

const std::string &Config::getIPAddress(void) const { return (_ipAddress); }

const int &Config::getPortNumber(void) const { return (_portNumber); }

const std::map<std::string, Location> &Config::getLocationMap(void) const {
  return (_locations);
}

bool Config::isLocationExist(const std::string &path) const {
  if (_locations.find(path) == _locations.end())
    return (false);
  else
    return (true);
}

const Location &Config::getLocation(const std::string &urlPath) const {
  const_location_iterator iterator;
  const_location_iterator endPoint = _locations.end();
  std::string key;
  std::size_t startIndex, endIndex;

  iterator = _locations.find(urlPath);
  if (iterator != endPoint)
    return (iterator->second);
  for (iterator = _locations.begin(); iterator != endPoint; iterator++) {
    key = iterator->first;
    if (key[0] == '~') {
      key = key.substr(1);
      startIndex = urlPath.find(key);
      if (startIndex != std::string::npos) {
        endIndex = startIndex + key.size();
        if (urlPath[endIndex] == '/' || urlPath.size() == endIndex)
          return (iterator->second);
      }
    }
  }
  if (urlPath == "/")
    return (*this);
  else
    throw(std::string("Error: Can't find locaiton for: " + urlPath));
}

std::set<std::string> Config::_getDirectiveSet(void) const {
  std::set<std::string> directiveSet = Location::_getDirectiveSet();

  directiveSet.insert(LISTEN_DIRECTIVE);
  directiveSet.insert(SERVER_NAME_DIRECTIVE);
  directiveSet.insert(LOCATION_DIRECTIVE);
  return (directiveSet);
}

void Config::_setFunctionPTRSet(void) {
  _configFunctions[LISTEN_DIRECTIVE] = &Config::_setListenData;
  _configFunctions[SERVER_NAME_DIRECTIVE] = &Config::_setServerName;
  _configFunctions[LOCATION_DIRECTIVE] = &Config::_setLocationBlock;
}

void Config::_setDirectiveData(void) {
  std::string directive = _configParser.getDirective();

  _setParameters();
  if (_locationFunctions.find(directive) != _locationFunctions.end())
    (this->*_locationFunctions[directive])();
  else
    (this->*_configFunctions[directive])();
}

void Config::_setListenData(void) {
  std::vector<std::string> listenData;

  listenData = ConfigUtil::splitString(_parameters[0], ':');
  _ipAddress = listenData[0];
  _portNumber = std::atoi(listenData[1].c_str());
}

void Config::_setServerName(void) { _serverName = _parameters[0]; }

void Config::_setLocationBlock(void) {
  std::string locationResource = _parameters[0];

  if (_locationBlocks.find(locationResource) != _locationBlocks.end())
    throw(
        std::runtime_error("Duplicate location resource: " + locationResource));
  _locationBlocks[locationResource] = _configParser.getBlock();
}

void Config::_setLocation(void) {
  location_block_map::iterator iterator = _locationBlocks.begin();
  location_block_map::iterator endPoint = _locationBlocks.end();

  for (; iterator != endPoint; iterator++)
    _locations[iterator->first] =
        Location(iterator->first, iterator->second, *this);
}

void Config::_checkConfigValidation(void) const {
  if (_ipAddress == "" || _portNumber == 0)
    throw(std::runtime_error(
        "Server configuration doesn't have any listen data"));
  if (_rootDirectory == "")
    throw(std::runtime_error(
        "Server configuration doesn't have a root directory"));
  if (_indexes.size() == 0)
    throw(std::runtime_error("Server configuration doesn't have any index"));
  if (_errorPages.size() == 0)
    throw(
        std::runtime_error("Server configuration doesn't set any error page"));
}
