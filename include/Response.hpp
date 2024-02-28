#pragma once

#include "ErrorResponse.hpp"
#include "Request.hpp"

class response
{
    public:
        response();
        ~response();

        std::string    setResponse(request _request);
        void	checkValidity();
        void	executeMethod();

        void	setEnvp(char **envp);
        char    **getEnvp() const;
        std::string	getPath(char **envp, std::string cmd);

        void    GET_HEAD();
        void    POST();
        void    DELETE();
        void    PUT();
        void    OPTIONS();
        void    TRACE();

        void		setResponseHeader(std::string key, std::string value);
		void		setResponseBody(std::string body);
		void		setResponse(std::string response);
        std::map<std::string, std::string> getResponseHeader() const;
        std::string getResponseBody() const;
		std::string	getResponse() const;
    private:
        response(const response &copy);
        response operator=(const response &copy);

        char **_envp;
        request _request;
        std::map<std::string, std::string> _responseHeader;
		std::string _responseBody;
		std::string _response;
        errorResponse _errorResponse;
    
};