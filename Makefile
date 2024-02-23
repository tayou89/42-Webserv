CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv

SRCS = main.cpp \
	   parse/ConfigBase.cpp \
	   parse/ConfigMain.cpp \
	   parse/Config.cpp \
	   parse/Location.cpp \
	   parse/ConfigParser.cpp \
	   parse/ConfigFile.cpp \
	   parse/ConfigStream.cpp \
	   parse/ConfigUtil.cpp \
	   parse/ParameterChecker.cpp \
	   server/ClientStat.cpp \
	   server/KqueueLoop.cpp \
	   server/WebServer.cpp \
	   protocol/executeMethod.cpp \
	   protocol/Protocol.cpp \
	   protocol/utils.cpp

INCS = include/ClientStat.hpp \
	   include/Config.hpp \
	   include/ConfigBase.hpp \
	   include/ConfigFile.hpp \
	   include/ConfigMain.hpp \
	   include/ConfigParser.hpp \
	   include/ConfigStream.hpp \
	   include/ConfigUtil.hpp \
	   include/IEventLoop.hpp \
	   include/IServer.hpp \
	   include/KqueueLoop.hpp \
	   include/Location.hpp \
	   include/ParameterChecker.hpp \
	   include/WebServer.hpp \
	   include/Protocol.hpp \
	   include/utils.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o:%.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	@rm -f $(OBJS)
	@echo "All object files removed"

fclean: clean
	@rm -f $(NAME)
	@echo "Executable file removed"

re:
	$(MAKE) fclean
	$(MAKE) all

.PHONY: all clean fclean re
