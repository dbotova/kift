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

OBJS=kift_client.o
TARGET=kift_client

INCLUDES := $(shell ${PKGCONFIG} --cflags sdl2)

CFLAGS += $(INCLUDES) -g
LFLAGS := $(shell ${PKGCONFIG} --libs sdl2)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS)

all: $(TARGET)
