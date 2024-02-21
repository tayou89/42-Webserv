CXX = c++
NAME = webserv
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS = srcs/main.cpp srcs/Protocol.cpp srcs/TestServer.cpp srcs/executeMethod.cpp srcs/utils.cpp
INCS = include
OBJS = $(SRCS:.cpp=.o)

.c.o :
	$(CXX) -c $(CXXFLAGS) -o $@ $^ -I $(INCS)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(NAME)

re :
	make fclean
	make all

.PHONY : all clean fclean re .c.o