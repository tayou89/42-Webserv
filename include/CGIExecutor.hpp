#ifndef CGI_EXECUTOR_HPP
#define CGI_EXECUTOR_HPP

#include "KqueueLoop.hpp"
#include "Location.hpp"
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

        CGIExecutor(const KqueueLoop &kqueueLoop, const Location &location,
                    const Protocol &protocol);
        // 비정상 종료시 1반환, 정상종료시 0반환
        int execute(const std::string &fileName);

    private:
        CGIExecutor(void);
        void        _setMetaVariables(void);
        void        _createPipeFD(void);
        void        _createProcess(void);
        void        _setPipeFD(void);

        std::string _getRequestMethod(void);
        std::string _getScriptName(void);
        std::string _getPathInfo(void);
        std::string _getRequestHeaderInfo(const std::string &infoName);

        KqueueLoop  _kqueueLoop;
        Location    _location;
        Protocol    _protocol;
        string_map  _metaVariables;
        int         _pipeFD[2];
        pid_t       _pid;
};

#endif