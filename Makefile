NAME = ircserv

SRC = main.cpp\
		Server.cpp \
		client/client.cpp \
		channel/channel.cpp\
#SRC = teste.cpp

# Colors
RED     = \033[1;31m
GREEN   = \033[1;32m
RESET   = \033[0m

CXX = c++ -g 
RM = rm -rf
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
AR = ar rcs

OBJDIR = .bin
OBJ = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
		@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
		clear
		echo "  ______ _______   _____ _____   _____ ";
		echo " |  ____|__   __| |_   _|  __ \ / ____|";
		echo " | |__     | |      | | | |__| | |     ";
		echo " |  __|    | |      | | |  _  /| |     ";
		echo " | |       | |     _| |_| | \ \| |____ ";
		echo " |_|       |_|    |_____|_|  \_\\_____|";
		echo "                                       ";
		echo "                                       ";
		echo "$(GREEN)Build completed. $(RESET)" ;

$(OBJDIR)/%.o: %.cpp
		mkdir -p $(@D)
		$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
		$(RM) $(OBJ)

fclean:
		$(RM) $(OBJ) $(NAME) $(OBJDIR)
		clear ;
		echo "$(RED)Cleaned project files/folders. $(RESET)" ;

re: fclean all

.SILENT: