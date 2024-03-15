#ifndef CONFIG_BASE_HPP
#define CONFIG_BASE_HPP

#include "ConfigFile.hpp"
#include "ConfigParser.hpp"
#include <string>

class ConfigBase
{
    public:
        typedef std::set<std::string> string_set;

        ConfigBase(void);
        virtual ~ConfigBase(void);
        ConfigBase(const ConfigBase &object);
        ConfigBase &operator=(const ConfigBase &object);

        ConfigBase(const std::string &configText, const string_set &directiveSet);
        ConfigBase(const char *configFilePath, const string_set &directiveSet);

    protected:
        virtual string_set _getDirectiveSet(void) const = 0;
        void               _setConfigData(void);
        bool               _parseDirectiveText(void);
        virtual void       _setDirectiveData(void) = 0;

        ConfigFile         _configFile;
        ConfigParser       _configParser;
};

#endif
