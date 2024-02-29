#ifndef CGI_EXECUTOR_HPP
#define CGI_EXECUTOR_HPP

#include "Config.hpp"
#include "Protocol.hpp"
#include <map>
#include <unistd.h>

class CGIExecutor
{
    public:
        typedef std::map<std::string, std::string> string_map;

        ~CGIExecutor(void);
        CGIExecutor(const CGIExecutor &object);
        CGIExecutor &operator=(const CGIExecutor &object);

        CGIExecutor(const Location &location, const Protocol &protocol);
        // 시스템 에러 발생시 1반환, 정상종료시 0반환
        int   execute(void);
        int   getReadFD(void) const;
        pid_t getPID(void) const;

    private:
        CGIExecutor(void);
        void        _setMetaVariables(void);
        void        _createPipeFD(void);
        void        _createProcess(void);
        void        _setPipeFD(void);
        void        _executeCGI(void);
        char      **_getEnvp(void) const;

        std::string _getRequestMethod(void);
        std::string _getScriptName(void);
        std::string _getPathInfo(void);
        std::string _getRequestHeaderInfo(const std::string &infoName);

        Location    _location;
        Protocol    _protocol;
        string_map  _metaVariables;
        int         _pipeFD[2];
        pid_t       _pid;
};

#endif