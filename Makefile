NAME = ircserv

SRC = main.cpp\
		Server.cpp \
		client/client.cpp \
		channel/channel.cpp\
#SRC = teste.cpp
	
CXX = c++ -g 
RM = rm -rf
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
AR = ar rcs

OBJ = $(SRC:%.cpp=%.o)

all: $(NAME)

$(NAME): $(OBJ)
		@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
		clear
		echo "  ______ _______   _____ _____   _____ ";
		echo " |  ____|__   __| |_   _|  __ \ / ____|";
		echo " | |__     | |      | | | |__) | |     ";
		echo " |  __|    | |      | | |  _  /| |     ";
		echo " | |       | |     _| |_| | \ \| |____ ";
		echo " |_|       |_|    |_____|_|  \_\\_____|";
		echo "                                       ";
		echo "                                       ";

clean:
		@$(RM) $(OBJ)
		
fclean: clean
		@$(RM) $(NAME)
		
re: fclean all

.SILENT: