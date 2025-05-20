### config
NAME		= webserv
CXX			= c++
CFLAGS		= -std=c++98 -Wall -Werror -Wextra
DEPFLAGS	= -MMD -MP
AUTHOR		= elleroux
DATE		= 2025/03/31 20:11:19
OS          = $(shell uname)


### program files
FILE_EXTENSION	= .cpp
SRCS_PATH		= ./src
INCLUDE_PATH	= ./inc
OBJ_PATH		= ./.obj
SRCS			= main.cpp \
				Request.cpp \
				Reponse.cpp \
				Server.cpp
ifeq ($(OS), Darwin)
	SRCS += Kqueue.cpp
else
	SRCS += Epoll.cpp
endif
HEADERS			= $(wildcard $(INCLUDE_PATH)/*.hpp)

### objects definition
OBJS = $(patsubst $(SRCS_PATH)/%$(FILE_EXTENSION),$(OBJ_PATH)/%.o,$(SRCS))
DEPS = $(patsubst $(SRCS_PATH)/%$(FILE_EXTENSION),$(OBJ_PATH)/%.d,$(SRCS))

### Makefile rules
all: $(NAME)

$(NAME): ${OBJS}
	$(CXX) $(CFLAGS) -o $@ ${OBJS}

$(OBJ_PATH)/%.o: $(SRCS_PATH)/%$(FILE_EXTENSION) $(HEADERS) Makefile
	mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(DEPFLAGS) -c $< -o $@ -I$(INCLUDE_PATH)

clean:
	@rm -fr $(OBJ_PATH)

fclean: clean
	@rm -rf $(NAME)

debug: fclean
debug: CFLAGS += -DDEBUG -g3
debug: all

re: fclean
	@$(MAKE) --no-print-directory all

-include $(DEPS)
.PHONY: all clean fclean re
