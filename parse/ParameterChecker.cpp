#include "../include/ParameterChecker.hpp"

ParameterChecker::ParameterChecker(void) { _setCheckFunctions(); }

ParameterChecker::~ParameterChecker(void) {}

ParameterChecker::ParameterChecker(const ParameterChecker &object) {
  *this = object;
}

ParameterChecker &ParameterChecker::operator=(const ParameterChecker &object) {
  if (this == &object)
    return (*this);
  _checkFunctions = object._checkFunctions;
  return (*this);
}

void ParameterChecker::checkParameter(const std::string &directive,
                                      const string_vector &parameters) {
  _setParameterVector(parameters);
  _checkParameterHasDelimeter();
  (this->*_checkFunctions[directive])();
}

void ParameterChecker::_setCheckFunctions(void) {
  _checkFunctions[SERVER_DIRECTIVE] = &ParameterChecker::_checkServerParameter;
  _checkFunctions[LISTEN_DIRECTIVE] = &ParameterChecker::_checkListenParameter;
  _checkFunctions[SERVER_NAME_DIRECTIVE] =
      &ParameterChecker::_checkServerNameParameter;
  _checkFunctions[ROOT_DIRECTIVE] = &ParameterChecker::_checkRootParameter;
  _checkFunctions[ERROR_PAGE_DIRECTIVE] =
      &ParameterChecker::_checkErrorPageParameter;
  _checkFunctions[CLIENT_MAX_DIRECTIVE] =
      &ParameterChecker::_checkClientMaxParameter;
  _checkFunctions[HEADER_MAX_DIRECTIVE] =
      &ParameterChecker::_checkHeaderMaxParameter;
  _checkFunctions[LOCATION_DIRECTIVE] =
      &ParameterChecker::_checkLocationParameter;
  _checkFunctions[INDEX_DIRECTIVE] = &ParameterChecker::_checkIndexParameter;
  _checkFunctions[AUTOINDEX_DIRECTIVE] =
      &ParameterChecker::_checkAutoindexParameter;
  _checkFunctions[REDIRECTION_DIRECTIVE] =
      &ParameterChecker::_checkReturnParameter;
  _checkFunctions[LIMIT_EXCEPT_DIRECTIVE] =
      &ParameterChecker::_checkLimitExceptParameter;
  _checkFunctions[CGI_PASS_DIRECTIVE] =
      &ParameterChecker::_checkCGIPassParameter;
  _checkFunctions[INCLUDE_DIRECTIVE] =
      &ParameterChecker::_checkIncludeParameter;
  _checkFunctions[TYPES_DIRECTIVE] = &ParameterChecker::_checkTypesParameter;
}

void ParameterChecker::_setParameterVector(const string_vector &parameters) {
  _parameters = parameters;
  _parameterCount = parameters.size();
}

void ParameterChecker::_checkParameterHasDelimeter(void) const {
  std::string parameter;
  std::size_t i;

  for (i = 0; i < _parameterCount; i++) {
    parameter = _parameters[i];
    if (ConfigUtil::findDelimeter(parameter) != std::string::npos)
      throw(std::invalid_argument("Parameter has delimeter: " + parameter));
  }
}

void ParameterChecker::_checkServerParameter(void) const {
  if (_parameterCount > 0)
    throw(std::invalid_argument(
        "\'server\' directive must not have any parameter"));
}

void ParameterChecker::_checkServerNameParameter(void) const {
  if (_parameterCount != 1)
    throw(std::invalid_argument(
        "\'server_name\' directive needs one server name"));
}

void ParameterChecker::_checkListenParameter(void) const {
  std::vector<std::string> listenElements;
  std::string IPAddress, portNumber;

  if (_parameterCount == 0)
    throw(std::invalid_argument(
        "\'listen\' directive needs \'host:port number\'"));
  if (_parameterCount > 1)
    throw(std::invalid_argument("\'listen\' directive needs one parameter"));
  listenElements = ConfigUtil::splitString(_parameters[0], ':');
  if (listenElements.size() != 2)
    throw(std::invalid_argument(
        "\'listen\' parameter must be \'host:port number\'" + _parameters[0]));
  IPAddress = listenElements[0];
  portNumber = listenElements[1];
  if (ConfigUtil::isIPAddressFormat(IPAddress) == false)
    throw(std::invalid_argument("Wrong IP format: " + IPAddress));
  if (ConfigUtil::isNumberString(portNumber) == false)
    throw(std::invalid_argument(portNumber +
                                ": Port number must be consist of numbers"));
  if (ConfigUtil::isValidPortNumber(std::atoi(portNumber.c_str())) == false)
    throw(std::invalid_argument(portNumber +
                                ": Port number must be (1 ~ 65535)"));
}

void ParameterChecker::_checkRootParameter(void) const {
  if (_parameterCount == 0)
    throw(std::invalid_argument("\'root\' directive needs a parameter"));
  if (_parameterCount > 1)
    throw(std::invalid_argument(
        "Too many parameters: \'root\' directive need one parameter"));
}

void ParameterChecker::_checkClientMaxParameter(void) const {
  std::string sizeString;

  if (_parameterCount == 0)
    throw(std::invalid_argument(
        "\'client_max_body_size\' directive needs a parameter"));
  if (_parameterCount > 1)
    throw(std::invalid_argument("Too many parameters: \'client_max_body_size\' "
                                "directive need one parameter"));
  sizeString = _parameters[0];
  if (ConfigUtil::isSizeString(sizeString) == false)
    throw(std::invalid_argument("Wrong size format: " + sizeString));
  if (ConfigUtil::isConvertibleToSizeT(sizeString) == false)
    throw(std::invalid_argument("Too big size: " + sizeString));
}

void ParameterChecker::_checkHeaderMaxParameter(void) const {
  std::string sizeString;

  if (_parameterCount == 0)
    throw(std::invalid_argument(
        "\'large_client_header_buffers\' directive needs a parameter"));
  if (_parameterCount > 1)
    throw(std::invalid_argument(
        "Too many parameters: \'large_client_header_buffers\' directive need "
        "one parameter"));
  sizeString = _parameters[0];
  if (ConfigUtil::isSizeString(sizeString) == false)
    throw(std::invalid_argument("Wrong size format: " + sizeString));
  if (ConfigUtil::isConvertibleToSizeT(sizeString) == false)
    throw(std::invalid_argument("Too big size: " + sizeString));
}

void ParameterChecker::_checkLocationParameter(void) const {
  std::string element;
  char firstCharacter;

  if (_parameterCount == 0)
    throw(std::invalid_argument("\'location\' directive needs a parameter"));
  if (_parameterCount > 1)
    throw(std::invalid_argument(
        "Too many parameters: \'location\' directive need one parameter"));
  element = _parameters[0];
  firstCharacter = element[0];
  if (firstCharacter != '/' && firstCharacter != '~')
    throw(std::invalid_argument(
        ": \'location\' parameter must start with \'/\' or \'~\'" + element));
}

void ParameterChecker::_checkIndexParameter(void) const {
  if (_parameterCount == 0)
    throw(std::invalid_argument(
        "Coudln't find any parameter for \'index\' directive"));
}

void ParameterChecker::_checkAutoindexParameter(void) const {
  std::string element;

  if (_parameterCount == 0)
    throw(std::invalid_argument(
        "\'autoindex\' directive needs a parameter(\"on\" or \"off\""));
  if (_parameterCount > 1)
    throw(std::invalid_argument(
        "Too many parameters: \'autoindex\' directive need one parameter"));
  element = _parameters[0];
  if (element != "on" && element != "off")
    throw(std::invalid_argument(
        "Wrong \'autoindex\' parameter: must be \"on\" or \"off\""));
}

void ParameterChecker::_checkReturnParameter(void) const {
  if (_parameterCount == 0)
    throw(std::invalid_argument(
        "\'return\' directive needs a parameter(message or URL)"));
  if (_parameterCount > 1)
    throw(std::invalid_argument(
        "Too many parameters: \'return\' directive need one parameter"));
}

void ParameterChecker::_checkErrorPageParameter(void) const {
  std::string errorNumber, errorFile;

  if (_parameterCount < 2)
    throw(std::invalid_argument("Too few argument: \'error_page\' needs page "
                                "number and error file path"));
  if (_parameterCount > 2)
    throw(std::invalid_argument("Too many argument: \'error_page\' needs page "
                                "number and error file path"));
  errorNumber = _parameters[0];
  errorFile = _parameters[1];
  if (ConfigUtil::isNumberString(errorNumber) == false)
    throw(std::invalid_argument("Wrong error number: " + errorNumber));
  if (ConfigUtil::isValidErrorStatusCode(std::atoi(errorNumber.c_str())) ==
      false)
    throw(std::invalid_argument(": " + errorNumber +
                                " error number must be (300 ~ 600)"));
}

void ParameterChecker::_checkLimitExceptParameter(void) const {
  std::string parameter;
  std::size_t i;

  if (_parameterCount == 0)
    throw(std::invalid_argument(
        "\'limit_except\' directive needs http methods to limit"));
  for (i = 0; i < _parameterCount; i++) {
    parameter = _parameters[i];
    if (ConfigUtil::isHTTPMethod(parameter) == false)
      throw(std::invalid_argument("Invalid HTTP method: " + parameter));
  }
}

void ParameterChecker::_checkCGIPassParameter(void) const {
  std::string element;

  if (_parameterCount < 1)
    throw(std::invalid_argument(
        "\'cgi_pass\' needs one parameter(\'on\' or \'off\')"));
  if (_parameterCount > 1)
    throw(std::invalid_argument(
        "Too many parameters: \'cgi_pass\' needs one parameter"));
  element = _parameters[0];
  if (element != "on" && element != "off")
    throw(std::invalid_argument(
        "Invalid parameter: " + element +
        ": only \'on\' or \'off\' is needed for cgi_pass"));
}

void ParameterChecker::_checkIncludeParameter(void) const {
  std::string element;

  if (_parameterCount < 1)
    throw(std::invalid_argument("\'include\' needs file path to include"));
  if (_parameterCount > 1)
    throw(std::invalid_argument(
        "Too many parameters: \'include\' needs one parameter"));
  element = _parameters[0];
  if (ConfigUtil::isExistingFile(element) == false)
    throw(std::invalid_argument("File doesn't exist: " + element));
  if (ConfigUtil::isReadableFile(element) == false)
    throw(std::invalid_argument("Can't read file: " + element));
}

void ParameterChecker::_checkTypesParameter(void) const {
  if (_parameterCount != 0)
    throw(std::invalid_argument("\'types\' deosn't need any parameter"));
}
