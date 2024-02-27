#include "CGIExecutor.hpp"
#include <cerrno>
#include <cstring>

CGIExecutor::CGIExecutor(void)
{
}

CGIExecutor::~CGIExecutor(void)
{
}

CGIExecutor::CGIExecutor(const CGIExecutor &object)
{
    *this = object;
}

CGIExecutor &CGIExecutor::operator=(const CGIExecutor &object)
{
    if (this == &object)
        return (*this);
    _location = object._location;
    _protocol = object._protocol;
    return (*this);
}

CGIExecutor::CGIExecutor(const KqueueLoop &kqueueLoop, const Location &location,
                         const Protocol &protocol)
    : _kqueueLoop(kqueueLoop), _location(location), _protocol(protocol)
{
    _setMetaVariables();
}

int CGIExecutor::execute(const std::string &fileName)
{
    try
    {
        _createPipeFD();
        _createProcess();
        _setPipeFD();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return (1);
    }
    return (0);
}

void CGIExecutor::_setMetaVariables(void)
{
    _metaVariables["REQUEST_METHOD"] = _getRequestMethod();
    _metaVariables["SCRIPT_NAME"]    = _getScriptName();
    _metaVariables["PATH_INFO"]      = _getPathInfo();
    _metaVariables["CONTENT_TYPE"]   = _getRequestHeaderInfo("Content-Type");
    _metaVariables["CONTENT_LENGTH"] = _getRequestHeaderInfo("Content-Length");
}

std::string CGIExecutor::_getRequestMethod(void)
{
    return (_protocol.getRequestMethod());
}

std::string CGIExecutor::_getScriptName(void)
{
    std::string locationPath = (_location.getLocationPath()).substr(1);
    std::string uri          = _protocol.getRequestURI();
    size_t      scriptEnd    = uri.find(locationPath) + locationPath.size();

    return (uri.substr(0, scriptEnd));
}

std::string CGIExecutor::_getPathInfo(void)
{
    std::string locationPath = (_location.getLocationPath()).substr(1);
    std::string uri          = _protocol.getRequestURI();
    size_t      scriptEnd    = uri.find(locationPath) + locationPath.size();

    return (uri.substr(scriptEnd));
}

std::string CGIExecutor::_getRequestHeaderInfo(const std::string &infoName)
{
    std::map<std::string, std::string>           headerEntry;
    std::map<std::string, std::string>::iterator iterator;

    headerEntry = _protocol.getRequestHeader();
    iterator    = headerEntry.find(infoName);
    if (iterator == headerEntry.end())
        return ("");
    else
        return (iterator->second);
}

void CGIExecutor::_createPipeFD(void)
{
    if (pipe(_pipeFD) == -1)
        throw(std::runtime_error(std::string("pipe: ") + std::strerror(errno)));
}

void CGIExecutor::_createProcess(void)
{
    _pid = fork();
    if (_pid == -1)
        throw(std::runtime_error(std::string("fork: ") + std::strerror(errno)));
}

void CGIExecutor::_setPipeFD(void)
{
    if (_pid == 0)
    {
        close(_pipeFD[0]);
        if (dup2(_pipeFD[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "Error: dup2: " << std::strerror(errno) << '\n';
            exit(1);
        }
        close(_pipeFD[1]);
    }
    else
        close(_pipeFD[1]);
}