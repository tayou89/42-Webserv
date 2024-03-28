#include "../include/MimeType.hpp"
#include <iostream>
#include <sstream>

MimeType::MimeType(void) : _defaultType("application/octet-stream") {}

MimeType::~MimeType(void) {}

MimeType::MimeType(const MimeType &object) { *this = object; }

MimeType &MimeType::operator=(const MimeType &object) {
  if (this == &object)
    return (*this);
  _mimeTypeMap = object._mimeTypeMap;
  return (*this);
}

MimeType::MimeType(const std::string &typesBlock)
    : ConfigBase(typesBlock, _getDirectiveSet()),
      _defaultType("application/octet-stream") {
  _setConfigData();
}

const std::string &MimeType::getType(const std::string &filePath) const {
  type_map::const_iterator iterator;
  type_map::const_iterator endPoint = _mimeTypeMap.end();
  std::string fileExtension;

  for (iterator = _mimeTypeMap.begin(); iterator != endPoint; iterator++) {
    if (ConfigUtil::isFileExtension(filePath, iterator->first) == true)
      return (iterator->second);
  }
  return (_defaultType);
}

std::set<std::string> MimeType::_getDirectiveSet(void) const {
  std::set<std::string> directiveSet;

  return (directiveSet);
}

void MimeType::_setDirectiveData(void) {
  std::vector<std::string> fileExtensions = _configParser.getParameters();
  std::string mimeType = _configParser.getDirective();
  std::size_t fileExtensionCount = fileExtensions.size();
  std::size_t i;

  for (i = 0; i < fileExtensionCount; i++)
    _mimeTypeMap[fileExtensions[i]] = mimeType;
}
