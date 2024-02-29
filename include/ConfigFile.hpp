#ifndef CONFIG_FILE_HPP
# define CONFIG_FILE_HPP

# include <string>
# include <fstream>

class	ConfigFile
{
	public:
		ConfigFile(void);
		ConfigFile(const char *configFilePath);
		~ConfigFile(void);
		ConfigFile(const ConfigFile &object);
		ConfigFile	&operator=(const ConfigFile &object);

		std::string	getFileString(void);
	
	private:
		void	_setFilePath(const char *configFilePath);
		void	_openFile(void);
		void	_closeFile(void);

		std::string		_filePath;
		std::ifstream	_fileStream;
};

#endif