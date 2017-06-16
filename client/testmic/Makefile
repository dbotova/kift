SHELL := /bin/bash
BINDIR := ../../../install/bin
PATH := $(BINDIR):$(PATH)
PKGCONFIG=${BINDIR}/pkg-config

OBJS=test_mic.o
TARGET=mict

INCLUDES := $(shell ${PKGCONFIG} --cflags sdl2)

CFLAGS += $(INCLUDES)
LFLAGS := $(shell ${PKGCONFIG} --libs sdl2)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS)

all: $(TARGET)
