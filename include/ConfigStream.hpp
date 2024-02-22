#ifndef CONFIG_STREAM_HPP
# define CONFIG_STREAM_HPP

#include <sstream>
#include <fstream>
#include "ConfigUtil.hpp"

class	ConfigStream : public std::stringstream
{
	public:
		ConfigStream(void);
		~ConfigStream(void);
		ConfigStream(const ConfigStream	&object);
		ConfigStream	&operator=(const ConfigStream &object);
		
		ConfigStream(std::string configText);
		void	init(void);
		void	pushFront(const std::string &string);

	private:
};

#endif