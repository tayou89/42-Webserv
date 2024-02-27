#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include "ConfigUtil.hpp"
#include <fstream>
#include <string>
#include <vector>

typedef struct FileStatus
{
        bool isAccessible;
        bool isReadable;
        bool isWritable;
        bool isExecutable;
} FileStatus;

class ConfigFile
{
    public:
        typedef std::vector<std::string> string_vector;

        ConfigFile(void);
        ~ConfigFile(void);
        ConfigFile(const ConfigFile &object);
        ConfigFile &operator=(const ConfigFile &object);

        ConfigFile(const char *configFilePath);
        std::string getString(void);
        std::string getPath(void) const;
        void        set(const std::string &dirPath, const string_vector &fileList);
        bool        isAccessible(void) const;
        bool        isReadable(void) const;
        bool        isWritable(void) const;
        bool        isExecutable(void) const;

    private:
        void          _setFilePath(const char *configFilePath);
        void          _openFile(void);
        void          _closeFile(void);
        void          _checkDirectoryStatus(void);
        void          _findIndexFile(const string_vector &fileList);
        void          _checkFileStatus(void);
        void          _initializeFileStatus(void);

        std::string   _directory;
        std::string   _path;
        std::ifstream _stream;
        FileStatus    _status;
};

#endif