#include "../include/ConfigFile.hpp"
#include <sstream>

ConfigFile::ConfigFile(void) {}

ConfigFile::~ConfigFile(void)
{
	_closeFile();
	_closeDirectory();
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
	: _dirPointer(NULL), _dirEntry(NULL)
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

std::string ConfigFile::getExecutableFile(const std::string &dirPath, const string_vector &fileList)
{
	try
	{
		_setDirectoryPath(dirPath);
		_openDirectory();
	}
	catch (const std::exception &e)
	{
	}
}

void	ConfigFile::_setDirectoryPath(const std::string &dirPath)
{
	_dirPath = dirPath;
}

void	ConfigFile::_openDirectory(void)
{
	_dirPointer = opendir(_dirPath.c_str());
	if (_dirPointer == NULL)
}

void ConfigFile::_closeFile(void)
{
	if (_fileStream.is_open() == true)
		_fileStream.close();
}

void ConfigFile::_closeDirectory(void)
{
	if (_dirPointer != NULL)
		closedir(_dirPointer);
}