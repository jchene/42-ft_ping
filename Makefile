NAME    = ft_ping
BIN_DIR = bin

SRCS    =	src/ft_ping.c \
			src/parsing/parsing.c \
			src/mutex/mutex_get.c \
			src/mutex/mutex_set.c \
			src/threads/parent.c \
			src/threads/sender.c \
			src/threads/receiver.c \
			src/utils/utils.c

OBJ_DIR = obj
OBJS    = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c99
SAN_FL	= -fsanitize=thread
LIBS    = -lm -lpthread

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJS)
	@echo "Linking object files"
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LIBS)
	@echo "Compilation ended : $(BIN_DIR)/$(NAME) was created."

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "Compilation of $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning local files"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Full cleaning"
	@rm -rf $(BIN_DIR)

re: fclean all

san: fclean
	$(MAKE) CFLAGS="$(CFLAGS) $(SAN_FL)" all

setcap: fclean
	$(MAKE) all
	@sudo setcap cap_net_raw+ep $(BIN_DIR)/$(NAME)

.PHONY: all clean fclean re san setcap
