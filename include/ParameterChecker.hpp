#ifndef PARAMETER_CHECKER_HPP
# define PARAMETER_CHECKER_HPP

# include <string>
# include <vector>
# include <map>
# include "ConfigUtil.hpp"

class 	ParameterChecker
{
	typedef std::vector<std::string>	string_vector;
	typedef void (ParameterChecker::*FunctionPTR) (void) const;

	public:
		ParameterChecker(void);
		~ParameterChecker(void);
		ParameterChecker(const ParameterChecker &object);
		ParameterChecker	&operator=(const ParameterChecker &object);

		void	checkParameter(const std::string &directive, const string_vector &parameters);

	private:
		void	_setCheckFunctions(void);
		void	_setParameterVector(const string_vector &parameters);
		void	_checkParameterHasDelimeter(void) const;
		void	_checkServerParameter(void) const;
		void	_checkListenParameter(void) const;
		void	_checkServerNameParameter(void) const;
		void	_checkRootParameter(void) const;
		void	_checkErrorPageParameter(void) const;
		void	_checkClientMaxParameter(void) const;
		void	_checkHeaderMaxParameter(void) const;
		void	_checkLocationParameter(void) const;
		void	_checkIndexParameter(void) const;
		void	_checkAutoindexParameter(void) const;
		void	_checkReturnParameter(void) const;
		void	_checkLimitExceptParameter(void) const;

		size_t								_parameterCount;	
		std::vector<std::string>			_parameters;
		std::map<std::string, FunctionPTR>	_checkFunctions;
};

#endif