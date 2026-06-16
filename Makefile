CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

NAME = ircserv

SRC = src/main.cpp \
	  src/message.cpp \
	  src/utils.cpp \
	  src/errorMessage.cpp \
	  src/commands/Invite.cpp \
	  src/commands/Join.cpp \
	  src/commands/Kick.cpp \
	  src/commands/Mode.cpp \
	  src/commands/Privmsg.cpp \
	  src/commands/Registration.cpp \
	  src/commands/Topic.cpp \
	  src/core/Channel.cpp \
	  src/core/Client.cpp \
	  src/core/Server.cpp \

OBJ_DIR = obj
OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

