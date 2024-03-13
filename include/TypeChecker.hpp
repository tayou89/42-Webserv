#ifndef TYPE_CHECKER_HPP
#define TYPE_CHECKER_HPP

#include <map>
#include <string>

class TypeChecker
{
    public:
        TypeChecker(void);
        ~TypeChecker(void);
        TypeChecker(const TypeChecker &object);
        TypeChecker &operator=(const TypeChecker &object);

    private:
        void                               _setMimeType(void);
        void                               _setTextType(void);
        void                               _setImageType(void);
        void                               _setApplicationType(void);
        std::map<std::string, std::string> _mimeTypeMap;
};

#endif