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

SHELL := /bin/bash
BINDIR := ../../install/bin
PATH := $(BINDIR):$(PATH)
PKGCONFIG=${BINDIR}/pkg-config

OBJS=	kift_client.o \
		recognize.o \
		parse_reply.o
TARGET=kift_client

INCLUDES := $(shell ${PKGCONFIG} --cflags sdl2)

CFLAGS += $(INCLUDES) -g -Wall -Werror -Wextra
LFLAGS := $(shell ${PKGCONFIG} --libs sdl2)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS)

all: $(TARGET)

clean: 
	rm -f kift_client.o $(TARGET)
