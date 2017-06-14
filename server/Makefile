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

SHELL := /bin/bash
BINDIR := ../../install/bin
PATH := $(BINDIR):$(PATH)
PKGCONFIG=${BINDIR}/pkg-config

OBJS = kift_server.o
TARGET = kift_server

MODELDIR = $(shell ${PKGCONFIG} --variable=modeldir pocketsphinx)

INCLUDES := $(shell ${PKGCONFIG} --cflags pocketsphinx sphinxbase)

CFLAGS := $(INCLUDES) -DMODELDIR=\"$(MODELDIR)\"
CPPFLAGS := $(INCLUDES) -DMODELDIR=\"$(MODELDIR)\"
LFLAGS := $(shell ${PKGCONFIG} --libs pocketsphinx sphinxbase)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS)

clean:
	echo $(PATH)
	$(RM) -f $(OBJS) $(TARGET)

# OBJS = kift_server.o

# HEADERS = kift_server.h

# CFLAGS += -Wall -Werror -Wextra

# NAME = kift_server

# all: $(NAME)

# $(NAME): $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# $(OBJ): $(HEADERS)

# clean: 
# 	rm -f $(OBJS)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean all
