#ifndef MIME_TYPE_HPP
#define MIME_TYPE_HPP

#include "ConfigBase.hpp"
#include <map>
#include <string>

class MimeType : public ConfigBase
{
    public:
        typedef std::map<std::string, std::string> type_map;

        MimeType(void);
        ~MimeType(void);
        MimeType(const MimeType &object);
        MimeType &operator=(const MimeType &object);

        MimeType(const std::string &typesBlock);
        const std::string &getType(const std::string &filePath) const;

    private:
        virtual string_set _getDirectiveSet(void) const;
        virtual void       _setDirectiveData(void);

        std::string        _defaultType;
        type_map           _mimeTypeMap;
};

#endif