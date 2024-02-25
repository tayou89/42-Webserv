#ifndef CONFIG_UTIL_HPP
# define CONFIG_UTIL_HPP

# include <string>
# include <stack>

# define	SERVER_DIRECTIVE		"server"
# define	LISTEN_DIRECTIVE		"listen"
# define	SERVER_NAME_DIRECTIVE	"server_name"
# define	ROOT_DIRECTIVE			"root"
# define	ERROR_PAGE_DIRECTIVE	"error_page"
# define	CLIENT_MAX_DIRECTIVE	"client_max_body_size"
# define	HEADER_MAX_DIRECTIVE	"large_client_header_buffers"
# define	LOCATION_DIRECTIVE		"location"
# define	INDEX_DIRECTIVE			"index"
# define	AUTOINDEX_DIRECTIVE		"autoindex"
# define	REDIRECTION_DIRECTIVE	"return"
# define	LIMIT_EXCEPT_DIRECTIVE	"limit_except"

class	ConfigUtil
{
	public:
		static bool		isIPAddressFormat(const std::string &string);
		static bool		isValidOctetNumber(const int &number);
		static bool		isNumberString(const std::string &string);
		static bool		isAlphabetNumberString(const std::string &string);
		static bool		isExtensionString(const std::string &string);
		static bool		isValidErrorStatusCode(const int &number);
		static bool		isValidPortNumber(const int &number);
		static size_t	convertToSizeT(const std::string &string);
		static bool		isConvertibleToSizeT(const std::string &string);
		static size_t	convertByteUnit(const char &byteUnit);
		static int		getStringCount(const std::string &string);
		static std::vector<std::string>	splitString(const std::string &string, const char &delimeter);
		static std::vector<std::string>	getStringVector(const std::string &string);

		static bool			isDelimeter(const char &character);
		static char			getParameterTerminator(const std::string &directive);
		static bool			isBlockDirective(const std::string &directive);
		static size_t		findDelimeter(const std::string &string);
		static std::string	convertIntToString(const int &integer);
		static bool			isSizeString(const std::string &string);
		static bool			isSizeUnit(const char &character);
		static void			removeComments(std::string &text);
		static bool			isHTTPMethod(const std::string &string);
		static bool			isFileExisted(const std::string &dirPath, const std::string &fileName);

		// //for test
		// static void			printStringVector(const std::vector<std::string> vector);
		// static void			printConfigData(const Config &config);
		// static void			printLocationData(const Location &location);

		// static void			printLocations(const std::map<std::string, Location> &locations);
		// static void			printIndexes(const std::vector<std::string> &indexes);
		// static void			printErrorPages(const std::map<int, std::string> &errorPages);
		// static void			printAcceptedMethods(const std::vector<std::string> &httpMethods);
};

#endif