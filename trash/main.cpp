// # include "ConfigMain.hpp"

// int	main(int argc, char *argv[])
// {
// 	ConfigMain	configMain(argv[1]);
// 	size_t		configCount;
// 	size_t		i;	

// 	(void) argc;
// 	std::vector<Config>	configs= configMain.getServerConfigs();
// 	configCount = configs.size();
// 	for (i = 0; i < configCount; i++)
// 		ConfigUtil::printConfigData(configs[i]);
// 	return (0);
// }