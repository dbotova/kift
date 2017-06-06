# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dbotova <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/06/05 16:35:13 by dbotova           #+#    #+#              #
#    Updated: 2017/06/05 16:35:15 by dbotova          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

OBJS = kift_server.o

HEADERS = kift_server.h

CFLAGS += -Wall -Werror -Wextra

NAME = kift_server

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ): $(HEADERS)

clean: 
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
