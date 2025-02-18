NAME    = ft_ping

SRCS    = src/main.c \
          src/parsing.c

OBJ_DIR = obj
OBJS    = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c99

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking object files..."
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@echo "Compilation ended : $(NAME) was created."

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation of $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning local files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Full cleaning..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
