#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include <string>
# include <vector>
# include <set>
# include "ConfigStream.hpp"
# include "ConfigUtil.hpp"
# include "ParameterChecker.hpp" 

class	ConfigParser
{
	public:
		ConfigParser(void);
		~ConfigParser(void);
		ConfigParser(const ConfigParser &object);
		ConfigParser	&operator=(const ConfigParser &object);

		ConfigParser(const std::string &configText, const std::set<std::string> directiveSet);
		const std::string				&getDirective(void) const;
		const std::vector<std::string>	&getParameters(void) const;
		const std::string				&getBlock(void) const;

		bool	parseDirectiveText(void);	

	private:
		void	_readDirective(void);	
		void	_checkDirectiveValidation(void) const;
		void	_readParameter(void);
		void	_setParameterVector(void);
		void	_checkParameterValidation(void);
		void	_readBlock(void);
		void	_initialize(void);

		ConfigStream				_configStream;
		std::set<std::string>		_directiveSet;
		ParameterChecker			_parameterChecker;

		std::string					_directive;
		std::string					_parameterString;
		std::string					_block;
		std::vector<std::string>	_parameters;
};

#endif