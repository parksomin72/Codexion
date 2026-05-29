NAME = codexion

CC = cc
CFLAGS = -g -Wall -Wextra -Werror -pthread

SRCS = coders/main.c \
       coders/parsing.c \
       coders/routine.c \
       coders/init.c \
       coders/request.c \
       coders/queue.c \
       coders/simulation.c \
       coders/cleanup.c \
       coders/dongles.c \
       coders/time.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c coders/main.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re