#include "../include/Location.hpp"
#include <dirent.h>
#include <iostream>
#include <unistd.h>

Location::Location(void)
    : _autoindex(false), _clientBodyMax(1024 * 1024),
      _clientHeaderMax(32 * 1024), _cgiPass(false) {}

Location::~Location(void) {}

Location::Location(const Location &object) { *this = object; }

Location &Location::operator=(const Location &object) {
  if (this == &object)
    return (*this);
  ConfigBase::operator=(object);
  _path = object._path;
  _locationFunctions = object._locationFunctions;
  _parameters = object._parameters;
  _parameterCount = object._parameterCount;
  _autoindex = object._autoindex;
  _clientBodyMax = object._clientBodyMax;
  _clientHeaderMax = object._clientHeaderMax;
  _rootDirectory = object._rootDirectory;
  _return = object._return;
  _indexes = object._indexes;
  _errorPages = object._errorPages;
  _acceptedMethods = object._acceptedMethods;
  _cgiPass = object._cgiPass;
  _mimeType = object._mimeType;
  return (*this);
}

Location::Location(const std::string &path, const std::string &text,
                   const Location &location)
    : ConfigBase(text, _getDirectiveSet()) {
  _path = path;
  _inheritUnconditionalData(location);
  _setFunctionPTRMap();
  _setConfigData();
  _inheritConditionalData(location);
}

Location::Location(const std::string &configText,
                   const string_set &directiveSet)
    : ConfigBase(configText, directiveSet), _autoindex(false),
      _clientBodyMax(1024 * 1024), _clientHeaderMax(32 * 1024),
      _cgiPass(false) {
  _setFunctionPTRMap();
}

const std::string &Location::getRootDirectory(void) const {
  return (_rootDirectory);
}

const std::string &Location::getReturn(void) const { return (_return); }

const std::size_t &Location::getClientBodyMax(void) const {
  return (_clientBodyMax);
}

const std::size_t &Location::getClientHeaderMax(void) const {
  return (_clientHeaderMax);
}

const bool &Location::getAutoIndex(void) const { return (_autoindex); }

const bool &Location::getCGIPass(void) const { return (_cgiPass); }

const std::vector<std::string> &Location::getIndexes(void) const {
  return (_indexes);
}

const error_page_map &Location::getErrorPageMap(void) const {
  return (_errorPages);
}

std::string Location::getErrorPage(const int &errorPageNumber) const {
  std::map<int, std::string>::const_iterator iterator;

  iterator = _errorPages.find(errorPageNumber);
  if (iterator == _errorPages.end())
    return ("");
  else
    return (_rootDirectory + iterator->second);
}

bool Location::isAcceptedHTTPMethod(const std::string &HTTPMethod) {
  string_vector::iterator start = _acceptedMethods.begin();
  string_vector::iterator end = _acceptedMethods.end();

  if (_acceptedMethods.size() == 0)
    return (true);
  if (std::find(start, end, HTTPMethod) != end)
    return (true);
  else
    return (false);
}

const string_vector &Location::getAcceptedMethods(void) const {
  return (_acceptedMethods);
}

const std::string &Location::getLocationPath(void) const { return (_path); }

void Location::setIndexFile(void) {
  try {
    _configFile.set(_rootDirectory, _indexes);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
  }
}

const ConfigFile &Location::getIndexFile(void) const { return (_configFile); }

const std::string &Location::getMimeType(const std::string &file) {
  return (_mimeType.getType(file));
}

std::set<std::string> Location::_getDirectiveSet(void) const {
  std::set<std::string> directiveSet;

  directiveSet.insert(ROOT_DIRECTIVE);
  directiveSet.insert(INDEX_DIRECTIVE);
  directiveSet.insert(AUTOINDEX_DIRECTIVE);
  directiveSet.insert(ERROR_PAGE_DIRECTIVE);
  directiveSet.insert(REDIRECTION_DIRECTIVE);
  directiveSet.insert(CLIENT_MAX_DIRECTIVE);
  directiveSet.insert(HEADER_MAX_DIRECTIVE);
  directiveSet.insert(LIMIT_EXCEPT_DIRECTIVE);
  directiveSet.insert(CGI_PASS_DIRECTIVE);
  directiveSet.insert(INCLUDE_DIRECTIVE);
  directiveSet.insert(TYPES_DIRECTIVE);
  return (directiveSet);
}

void Location::_setFunctionPTRMap(void) {
  _locationFunctions[ROOT_DIRECTIVE] = &Location::_setRootDirectory;
  _locationFunctions[INDEX_DIRECTIVE] = &Location::_setIndexes;
  _locationFunctions[AUTOINDEX_DIRECTIVE] = &Location::_setAutoindex;
  _locationFunctions[ERROR_PAGE_DIRECTIVE] = &Location::_setErrorPages;
  _locationFunctions[REDIRECTION_DIRECTIVE] = &Location::_setReturn;
  _locationFunctions[CLIENT_MAX_DIRECTIVE] = &Location::_setClientBodyMax;
  _locationFunctions[HEADER_MAX_DIRECTIVE] = &Location::_setClientHeaderMax;
  _locationFunctions[LIMIT_EXCEPT_DIRECTIVE] = &Location::_setAcceptedMethods;
  _locationFunctions[CGI_PASS_DIRECTIVE] = &Location::_setCGIPass;
  _locationFunctions[INCLUDE_DIRECTIVE] = &Location::_setInclude;
  _locationFunctions[TYPES_DIRECTIVE] = &Location::_setMimeType;
}

void Location::_setDirectiveData(void) {
  _setParameters();
  (this->*_locationFunctions[_configParser.getDirective()])();
}

void Location::_inheritUnconditionalData(const Location &location) {
  _rootDirectory = location._rootDirectory;
  _autoindex = location._autoindex;
  _clientBodyMax = location._clientBodyMax;
  _clientHeaderMax = location._clientHeaderMax;
  _return = location._return;
  _errorPages = location._errorPages;
  _cgiPass = location._cgiPass;
  _mimeType = location._mimeType;
}

void Location::_inheritConditionalData(const Location &location) {
  if (_indexes.size() == 0 && _autoindex == false)
    _indexes = location._indexes;
}

void Location::_setParameters(void) {
  _parameters = _configParser.getParameters();
  _parameterCount = _parameters.size();
}

void Location::_setRootDirectory(void) { _rootDirectory = _parameters[0]; }

void Location::_setIndexes(void) {
  std::size_t i;

  if (_indexes.empty() == false)
    _indexes.clear();
  for (i = 0; i < _parameterCount; i++)
    _indexes.push_back(_parameters[i]);
}

void Location::_setErrorPages(void) {
  std::string parameter = _parameters[1];

  if (parameter[0] != '/')
    parameter = '/' + parameter;
  _errorPages[std::atoi(_parameters[0].c_str())] = parameter;
}

void Location::_setAutoindex(void) {
  if (_parameters[0] == "on")
    _autoindex = true;
  else
    _autoindex = false;
}

void Location::_setReturn(void) { _return = _parameters[0]; }

void Location::_setClientBodyMax(void) {
  std::string sizeString = _parameters[0];
  std::size_t number = ConfigUtil::convertToSizeT(sizeString);
  std::size_t unit =
      ConfigUtil::convertByteUnit(sizeString[sizeString.size() - 1]);

  _clientBodyMax = number * unit;
}

void Location::_setClientHeaderMax(void) {
  std::string sizeString = _parameters[0];
  std::size_t number = ConfigUtil::convertToSizeT(sizeString);
  std::size_t unit =
      ConfigUtil::convertByteUnit(sizeString[sizeString.size() - 1]);

  _clientHeaderMax = number * unit;
}

void Location::_setAcceptedMethods(void) { _acceptedMethods = _parameters; }

void Location::_setCGIPass(void) {
  if (_parameters[0] == "on")
    _cgiPass = true;
  else
    _cgiPass = false;
}

void Location::_setInclude(void) {
  ConfigFile includeFile(_parameters[0].c_str());

  _configParser.addStringFront(includeFile.getString());
}

void Location::_setMimeType(void) {
  _mimeType = MimeType(_configParser.getBlock());
}
