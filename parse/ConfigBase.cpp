#include "../include/ConfigBase.hpp"
#include <iostream>

ConfigBase::ConfigBase(void) {}

ConfigBase::~ConfigBase(void) {}

ConfigBase::ConfigBase(const ConfigBase &object) { *this = object; }

ConfigBase &ConfigBase::operator=(const ConfigBase &object) {
  if (this == &object)
    return (*this);
  _configParser = object._configParser;
  return (*this);
}

ConfigBase::ConfigBase(const std::string &configText,
                       const string_set &directiveSet)
    : _configParser(configText, directiveSet) {}

ConfigBase::ConfigBase(const char *configFilePath,
                       const string_set &directiveSet)
    : _configFile(configFilePath),
      _configParser(_configFile.getString(), directiveSet) {}

void ConfigBase::_setConfigData(void) {
  while (_parseDirectiveText())
    _setDirectiveData();
}

bool ConfigBase::_parseDirectiveText(void) {
  return (_configParser.parseDirectiveText());
}
