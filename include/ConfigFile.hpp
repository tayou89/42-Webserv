#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <dirent.h>
#include <fstream>
#include <string>

class ConfigFile
{
    public:
        typedef std::vector<std::string> string_vector;

        ConfigFile(void);
        ConfigFile(const char *configFilePath);
        ~ConfigFile(void);
        ConfigFile(const ConfigFile &object);
        ConfigFile &operator=(const ConfigFile &object);

        std::string getFileString(void);
        std::string getExecutableFile(const std::string &dirPath, const string_vector &fileList);
    private:
        void _setFilePath(const char *configFilePath);
        void _openFile(void);
        void _closeFile(void);

        void _setDirectoryPath(const std::string &dirPath);
        void _openDirectory(void);
        void _closeDirectory(void);

        std::string   _filePath;
        std::ifstream _fileStream;

        std::string    _dirPath;
        DIR           *_dirPointer;
        struct dirent *_dirEntry;

        int _errorCode;
};

#endif