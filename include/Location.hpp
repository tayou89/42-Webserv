#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "ConfigBase.hpp"
# include <set>
# include <map>

typedef std::map<int, std::string>	error_page_map;
typedef std::vector<std::string>	string_vector;

class	Location : public ConfigBase
{
	public:
		typedef	void (Location::*LocationFunctionPTR)(void);
		typedef std::map<std::string, LocationFunctionPTR>	location_function_map;

		Location(void);
		~Location(void);
		Location(const Location &object);
		Location	&operator=(const Location &object);

		Location(const std::string &locationBlock, const Location &location);
		Location(const std::string &configText, const string_set &directiveSet); 
		const bool				&getAutoIndex(void) const;
		const size_t			&getClientBodyMax(void) const;
		const size_t			&getClientHeaderMax(void) const;
		const std::string		&getRootDirectory(void) const;
		const std::string		&getReturn(void) const;
		const string_vector		&getIndexes(void) const;
		const error_page_map	&getErrorPageMap(void) const;
		std::string				getErrorPage(const int &errorPageNumber) const;
		bool					isIndexExist(const std::string &path) const;
		//매게변수로 입력된 HTTP method가 서버 설정에 허용되는 메소드인지 확인하는 함수
		bool					isAcceptedHTTPMethod(const std::string &HTTPMethod);
		const string_vector		&getAcceptedMethods(void) const;
	
	protected:
		virtual string_set		_getDirectiveSet(void) const;
		virtual void			_setFunctionPTRMap(void);
		virtual void			_setDirectiveData(void);
		void					_setParameters(void);
		void					_setRootDirectory(void);
		void					_setIndexes(void);
		void					_setAutoindex(void);
		void					_setErrorPages(void);
		void					_setReturn(void);
		void					_setClientBodyMax(void);
		void					_setClientHeaderMax(void);
		void					_setAcceptedMethods(void);

		bool					_autoindex;
		size_t					_clientBodyMax;
		size_t					_clientHeaderMax;
		std::string				_rootDirectory;
		std::string				_return;
		string_vector			_indexes;
		error_page_map			_errorPages;
		string_vector			_acceptedMethods;

		location_function_map	_locationFunctions;
		string_vector			_parameters;
		size_t					_parameterCount;
};

#endif