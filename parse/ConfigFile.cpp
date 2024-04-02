#include "../include/ConfigFile.hpp"
#include <iostream>
#include <sstream>

ConfigFile::ConfigFile(void) {
  _status.isAccessible = true;
  _status.isReadable = false;
  _status.isWritable = false;
  _status.isExecutable = false;
}

ConfigFile::~ConfigFile(void) { _closeFile(); }

ConfigFile::ConfigFile(const ConfigFile &object) { *this = object; }

ConfigFile &ConfigFile::operator=(const ConfigFile &object) {
  if (this == &object)
    return (*this);
  _closeFile();
  _directory = object._directory;
  _path = object._path;
  _openFile();
  _status.isAccessible = object._status.isAccessible;
  _status.isReadable = object._status.isReadable;
  _status.isWritable = object._status.isWritable;
  _status.isExecutable = object._status.isExecutable;
  return (*this);
}

ConfigFile::ConfigFile(const char *configFilePath) {
  try {
    _setFilePath(configFilePath);
    _openFile();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    exit(1);
  }
}

void ConfigFile::_setFilePath(const char *configFilePath) {
  if (configFilePath == NULL)
    _path = "./config/default.conf";
  else
    _path = configFilePath;
}

void ConfigFile::_openFile(void) {
  _stream.open(_path);
  if (_stream.is_open() == false)
    throw(std::runtime_error("Opening file failed: " + _path));
}

std::string ConfigFile::getPath(void) const { return (_path); }

std::string ConfigFile::getString(void) {
  typedef std::istreambuf_iterator<char> buf_iterator;

  return (std::string(buf_iterator(_stream), buf_iterator()));
}

bool ConfigFile::isAccessible(void) const { return (_status.isAccessible); }

bool ConfigFile::isReadable(void) const { return (_status.isReadable); }

bool ConfigFile::isWritable(void) const { return (_status.isWritable); }

bool ConfigFile::isExecutable(void) const { return (_status.isExecutable); }

void ConfigFile::set(const std::string &dirPath,
                     const string_vector &fileList) {
  _directory = dirPath;
  _path.clear();
  _initializeFileStatus();
  _checkDirectoryStatus();
  _findIndexFile(fileList);
  _checkFileStatus();
}

void ConfigFile::_initializeFileStatus(void) {
  _status.isAccessible = true;
  _status.isReadable = false;
  _status.isWritable = false;
  _status.isExecutable = false;
}

void ConfigFile::_checkDirectoryStatus(void) {
  if (ConfigUtil::isExistingFile(_directory) == false)
    throw(std::runtime_error("Not found: " + _directory));
  if (ConfigUtil::isExecutableFile(_directory) == false) {
    _status.isAccessible = false;
    throw(std::runtime_error("Can't read: " + _directory));
  }
  if (ConfigUtil::isReadableFile(_directory) == false) {
    _status.isAccessible = false;
    throw(std::runtime_error("Not executable: " + _directory));
  }
}

void ConfigFile::_findIndexFile(const string_vector &fileList) {
  std::string fileName, filePath;
  std::size_t fileCount = fileList.size();
  std::size_t i;

  for (i = 0; i < fileCount; i++) {
    fileName = fileList[i];
    if (fileName[0] == '/')
      filePath = _directory + fileName;
    else
      filePath = _directory + '/' + fileName;
    if (ConfigUtil::isExistingFile(filePath) == true) {
      _path = filePath;
      return;
    }
  }
  throw(std::runtime_error("Can't find any index file from: " + _directory));
}

void ConfigFile::_checkFileStatus(void) {
  if (ConfigUtil::isExecutableFile(_path) == true)
    _status.isExecutable = true;
  if (ConfigUtil::isReadableFile(_path) == true)
    _status.isReadable = true;
  if (ConfigUtil::isWritableFile(_path) == true)
    _status.isWritable = true;
}

void ConfigFile::_closeFile(void) {
  if (_stream.is_open() == true)
    _stream.close();
}
