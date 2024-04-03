#ifndef CONFIG_MAIN_HPP
#define CONFIG_MAIN_HPP

#include "Config.hpp"
#include "ConfigBase.hpp"
#include <string>
#include <vector>

class ConfigMain : public ConfigBase {
public:
  ConfigMain(const char *configFilePath);
  ConfigMain(const ConfigMain &object);
  ConfigMain &operator=(const ConfigMain &object);
  ~ConfigMain(void);

  std::vector<Config> getServerConfigs(void) const;

private:
  ConfigMain(void);

  virtual string_set _getDirectiveSet(void) const;
  virtual void _setDirectiveData(void);
  void _setServerConfig(void);
  void _checkConfigMainException(void) const;

  std::vector<Config> _serverConfig;
};

#endif
