#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "ConfigBase.hpp"
#include "MimeType.hpp"
#include <map>
#include <set>

typedef std::map<int, std::string> error_page_map;
typedef std::vector<std::string> string_vector;

class Location : public ConfigBase {
public:
  typedef void (Location::*LocationFunctionPTR)(void);
  typedef std::map<std::string, LocationFunctionPTR> location_function_map;

  Location(void);
  virtual ~Location(void);
  Location(const Location &object);
  Location &operator=(const Location &object);

  Location(const std::string &path, const std::string &text,
           const Location &location);
  Location(const std::string &configText, const string_set &directiveSet);
  bool isAcceptedHTTPMethod(const std::string &HTTPMethod);
  void setIndexFile(void);
  const bool &getAutoIndex(void) const;
  const bool &getCGIPass(void) const;
  std::string getErrorPage(const int &errorPageNumber) const;
  const std::size_t &getClientBodyMax(void) const;
  const std::size_t &getClientHeaderMax(void) const;
  const ConfigFile &getIndexFile(void) const;
  const std::string &getRootDirectory(void) const;
  const std::string &getReturn(void) const;
  const std::string &getLocationPath(void) const;
  const std::string &getMimeType(const std::string &file);
  const string_vector &getIndexes(void) const;
  const string_vector &getAcceptedMethods(void) const;
  const error_page_map &getErrorPageMap(void) const;

protected:
  virtual string_set _getDirectiveSet(void) const;
  virtual void _setFunctionPTRMap(void);
  virtual void _setDirectiveData(void);
  void _inheritUnconditionalData(const Location &location);
  void _inheritConditionalData(const Location &location);
  void _setParameters(void);
  void _setRootDirectory(void);
  void _setIndexes(void);
  void _setAutoindex(void);
  void _setErrorPages(void);
  void _setReturn(void);
  void _setClientBodyMax(void);
  void _setClientHeaderMax(void);
  void _setAcceptedMethods(void);
  void _setCGIPass(void);
  void _setInclude(void);
  void _setMimeType(void);

  std::string _path;
  bool _autoindex;
  std::size_t _clientBodyMax;
  std::size_t _clientHeaderMax;
  std::string _rootDirectory;
  std::string _return;
  string_vector _indexes;
  error_page_map _errorPages;
  string_vector _acceptedMethods;
  bool _cgiPass;
  MimeType _mimeType;

  location_function_map _locationFunctions;
  string_vector _parameters;
  std::size_t _parameterCount;
};

#endif
