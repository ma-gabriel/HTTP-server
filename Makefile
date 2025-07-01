### config
NAME		= webserv
CXX			= c++
CFLAGS		= -std=c++98 -Wall -Werror -Wextra -g3
DEPFLAGS	= -MMD -MP
OS          = $(shell uname)


### program files
FILE_EXTENSION	= .cpp
SRCS_PATH		= ./src
INCLUDE_PATH	= ./inc
OBJ_PATH		= ./.obj
SRCS_FILES		= main.cpp \
                  Request.cpp \
                  Response.cpp \
                  Server.cpp \
                  Atributes.cpp \
                  Location.cpp \
                  Parser.cpp \
                  args.cpp \
                  ConfigurationServer.cpp \
                  utils/isSeparator.cpp \
                  utils/strNoCase.cpp \
                  utils/strIsdigit.cpp \
                  exceptions/LocationException.cpp \
                  Epoll.cpp \
                  CGI.cpp


ifeq ($(OS), Linux)
	CFLAGS += -DLINUX
endif

SRCS = $(addprefix $(SRCS_PATH)/, $(SRCS_FILES))
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
	@rm -rf static/CGI-scripts/uploads
	@rm -rf static/uploads/*
	@rm -rf $(NAME)

debug: fclean
debug: CFLAGS += -DDEBUG -g3
debug: all

re: fclean
	@$(MAKE) --no-print-directory all

-include $(DEPS)
.PHONY: all clean fclean re
