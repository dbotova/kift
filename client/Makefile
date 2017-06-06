# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dbotova <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/06/05 16:38:31 by dbotova           #+#    #+#              #
#    Updated: 2017/06/05 16:38:32 by dbotova          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

OBJS = kift_client.o

HEADERS = kift_client.h

CFLAGS += -Wall -Werror -Wextra

NAME = kift_client

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ): $(HEADERS)

clean: 
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
