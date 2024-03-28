#include "../include/ConfigMain.hpp"
#include <cstdlib>
#include <iostream>

ConfigMain::ConfigMain(void) {}

ConfigMain::~ConfigMain(void) {}

ConfigMain::ConfigMain(const ConfigMain &object) { *this = object; }

ConfigMain &ConfigMain::operator=(const ConfigMain &object) {
  if (this == &object)
    return (*this);
  ConfigBase::operator=(object);
  _configFile = object._configFile;
  _serverConfig = object._serverConfig;
  return (*this);
}

std::vector<Config> ConfigMain::getServerConfigs(void) const {
  return (_serverConfig);
}

ConfigMain::ConfigMain(const char *configFilePath)
    : ConfigBase(configFilePath, _getDirectiveSet()) {
  try {
    _setConfigData();
    _checkConfigMainException();
  } catch (const std::exception &exception) {
    std::cerr << "Error: " << exception.what() << '\n';
    std::exit(1);
  }
}

std::set<std::string> ConfigMain::_getDirectiveSet(void) const {
  std::set<std::string> directiveSet;

  directiveSet.insert("server");
  return (directiveSet);
}

void ConfigMain::_setDirectiveData(void) { _setServerConfig(); }

void ConfigMain::_setServerConfig(void) {
  _serverConfig.push_back(Config(_configParser.getBlock()));
}

void ConfigMain::_checkConfigMainException(void) const {
  std::size_t configCount = _serverConfig.size();

  if (configCount == 0)
    throw(std::runtime_error("Can't find server configuration"));
}
