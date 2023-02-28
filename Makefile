# -*- MakeFile -*-
ifeq ($(OS), Windows_NT)
detected_OS = Windows
CLEAN = del /s /q /f .\x64\bin\* .\x64\obj\* 
CFLAGS = -g -Wall -Werror -pthread -lpthread -static
SHARE = bin/comInterface.dll
else
detected_OS = $(shell uname)
CLEAN = -rm *.o x64/obj/* x64/bin/* 
CFLAGS =-g -Wall -Werror -pthread -lpthread
SHARE = bin/comInterface.so
endif

CC=gcc
SRC=src
OBJ=obj
BINDIR=bin
LIBS=$(BINDIR)/*
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

SRC64	=	x64/src

OBJ64	=	x64/obj

SRCS64=$(wildcard $(SRC64)/*.c)
OBJS64=$(patsubst $(SRC64)/%.c, $(OBJ64)/%.o, $(SRCS64))

SHARE64		=	x64/$(SHARE)

ifeq ($(detected_OS), Windows)
dir: 
	mkdir "x64/obj"
	mkdir "x64/bin"
else
dir: 
	mkdir -p x64/obj x64/bin
endif
	

all:$(dir) $(SHARE64)

$(SHARE64): $(OBJS64)
	$(CC) -fPIC -shared $(CFLAGS) $(OBJS64) -o $@

$(OBJ64)/%.o: $(SRC64)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(CLEAN)
