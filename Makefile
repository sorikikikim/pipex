NAME = pipex
HEADER = $(addsuffix .h, $(NAME))
CFLAGS = -Wall -Wextra -Werror
CC = gcc
RM = rm -f

SRCS =	pipex.c utils.c libft_utils1.c libft_utils2.c

OBJS = $(SRCS:.c=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(PTHREAD) $^ -o $@

%.o : %.c $(HEADER)
	$(CC) $(CFLAGS) $(PTHREAD) -c $< -o $@

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(NAME)

re : fclean all