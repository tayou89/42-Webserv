#include "../include/ConfigParser.hpp"
#include <iostream>
#include <stack>

ConfigParser::ConfigParser(void) {}

ConfigParser::~ConfigParser(void) {}

ConfigParser::ConfigParser(const ConfigParser &object) { *this = object; }

ConfigParser &ConfigParser::operator=(const ConfigParser &object) {
  if (this == &object)
    return (*this);
  _configStream = object._configStream;
  _directiveSet = object._directiveSet;
  _parameterChecker = object._parameterChecker;
  _directive = object._directive;
  _parameterString = object._parameterString;
  _block = object._block;
  _parameters = object._parameters;
  return (*this);
}

ConfigParser::ConfigParser(const std::string &configText,
                           const std::set<std::string> directiveSet)
    : _configStream(configText), _directiveSet(directiveSet) {}

const std::string &ConfigParser::getDirective(void) const {
  return (_directive);
}

const std::vector<std::string> &ConfigParser::getParameters(void) const {
  return (_parameters);
}

const std::string &ConfigParser::getBlock(void) const { return (_block); }

void ConfigParser::addStringFront(const std::string &string) {
  _configStream.pushFront(string);
}

bool ConfigParser::parseDirectiveText(void) {
  _initialize();
  _readDirective();
  if (_directive == "")
    return (false);
  _checkDirectiveValidation();
  _readParameter();
  _setParameterVector();
  _checkParameterValidation();
  if (ConfigUtil::isBlockDirective(_directive) == true)
    _readBlock();
  return (true);
}

void ConfigParser::_readDirective(void) {
  std::size_t delimeterIndex;

  if (!(_configStream >> _directive))
    return;
  delimeterIndex = ConfigUtil::findDelimeter(_directive);
  if (delimeterIndex != std::string::npos) {
    _configStream.pushFront(_directive.substr(delimeterIndex));
    _directive = _directive.substr(0, delimeterIndex);
  }
}

void ConfigParser::_checkDirectiveValidation(void) const {
  if (_parsingType == CONFIG) {
    if (_directiveSet.find(_directive) == _directiveSet.end())
      throw(std::runtime_error("Invalid directive: " + _directive));
  } else {
    if (ConfigUtil::isMimeTypeFormat(_directive) == false)
      throw(std::invalid_argument("Wrong MIME type format: " + _directive));
  }
}

void ConfigParser::_readParameter(void) {
  char terminator = ConfigUtil::getParameterTerminator(_directive);

  std::getline(_configStream, _parameterString, terminator);
}

void ConfigParser::_setParameterVector(void) {
  _parameters = ConfigUtil::getStringVector(_parameterString);
}

void ConfigParser::_checkParameterValidation(void) {
  if (_parsingType == CONFIG)
    _parameterChecker.checkParameter(_directive, _parameters);
  else {
    if (_parameters.size() == 0)
      throw(std::invalid_argument("No extension exeist for: " + _directive));
  }
}

void ConfigParser::_readBlock(void) {
  std::stack<char> braceStack;
  char blockStarter = '{', blockTerminator = '}';
  char character = 0;

  braceStack.push(character);
  while (_configStream.get(character)) {
    if (character == blockStarter)
      braceStack.push(character);
    if (character == blockTerminator) {
      braceStack.pop();
      if (braceStack.empty())
        return;
    }
    _block += character;
  }
}

void ConfigParser::_initialize(void) {
  _directive.clear();
  _parameterString.clear();
  _block.clear();
  _parameters.clear();
  if (_directiveSet.size() == 0)
    _parsingType = MIME;
  else
    _parsingType = CONFIG;
}
