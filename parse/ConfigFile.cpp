#include "../include/ConfigFile.hpp"
#include <sstream>

ConfigFile::ConfigFile(void) {}

ConfigFile::~ConfigFile(void)
{
	_closeFile();
}

ConfigFile::ConfigFile(const ConfigFile &object)
{
	*this = object;
}

ConfigFile &ConfigFile::operator=(const ConfigFile &object)
{
	if (this == &object)
		return (*this);
	_closeFile();
	_filePath = object._filePath;
	_openFile();
	return (*this);
}

ConfigFile::ConfigFile(const char *configFilePath)
{
	_setFilePath(configFilePath);
	_openFile();
}

void ConfigFile::_setFilePath(const char *configFilePath)
{
	if (configFilePath == NULL)
		_filePath = "./config/default.conf";
	else
		_filePath = configFilePath;
}

void ConfigFile::_openFile(void)
{
	_fileStream.open(_filePath);
	if (_fileStream.is_open() == false)
		throw(std::runtime_error("Opening file failed: " + _filePath));
}

std::string ConfigFile::getFileString(void)
{
	typedef std::istreambuf_iterator<char> buf_iterator;

	return (std::string(buf_iterator(_fileStream), buf_iterator()));
}

void ConfigFile::_closeFile(void)
{
	if (_fileStream.is_open() == true)
		_fileStream.close();
}
