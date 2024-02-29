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
    _location      = object._location;
    _protocol      = object._protocol;
    _metaVariables = object._metaVariables;
    _pipeFD[0]     = object._pipeFD[0];
    _pipeFD[1]     = object._pipeFD[1];
    _pid           = object._pid;
    return (*this);
}

CGIExecutor::CGIExecutor(const Location &location, const Protocol &protocol)
    : _location(location), _protocol(protocol)
{
    _setMetaVariables();
}

int CGIExecutor::execute(void)
{
    try
    {
        _createPipeFD();
        _createProcess();
        _setPipeFD();
        _executeCGI();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        if (_pid == 0)
            exit(1);
        else
            return (1);
    }
    if (_pid == 0)
        exit(0);
    else
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
            throw(std::runtime_error(std::string("dup2: ") + std::strerror(errno)));
        close(_pipeFD[1]);
    }
    else
        close(_pipeFD[1]);
}

void CGIExecutor::_executeCGI(void)
{
    if (_pid == 0)
    {
        ConfigFile CGIFile = _location.getIndexFile();
        char     **envp    = _getEnvp();
        char      *argv[2] = {const_cast<char *>(CGIFile.getPath().c_str()), NULL};

        if (execve(argv[0], argv, envp) == -1)
        {
            ConfigUtil::freeStringArray(envp);
            throw(std::runtime_error(std::string("execve: ") + std::strerror(errno)));
        }
    }
}

char **CGIExecutor::_getEnvp(void) const
{
    char                     **envp = new char *[_metaVariables.size() + 1];
    string_map::const_iterator iterator;
    string_map::const_iterator endPoint = _metaVariables.end();
    std::string                envString;
    size_t                     i = 0;

    for (iterator = _metaVariables.begin(); iterator != endPoint; iterator++)
    {
        envString = iterator->first + '=' + iterator->second;
        envp[i]   = new char[envString.size() + 1];
        std::copy(envString.begin(), envString.end(), envp[i]);
        envp[i][envString.size()] = '\0';
        ++i;
    }
    envp[i] = NULL;
    return (envp);
}

pid_t CGIExecutor::getPID(void) const
{
    return (_pid);
}

int CGIExecutor::getReadFD(void) const
{
    return (_pipeFD[0]);
}