# -*- MakeFile -*-
ifeq ($(OS), Windows_NT)
	detected_OS := Windows
	CLEAN = del /s /q /f .\x64\bin\* .\x64\obj\* .\x64_u\bin\* .\x64_u\obj\*  .\x86\bin\* .\x86\obj\*  .\x86_u\bin\* .\x86_u\obj\*
	CFLAGS = -g -Wall -Werror -pthread -lpthread -static
	SHARE = bin/comInterface.dll
else
	detected_OS := $(shell uname)
	CLEAN= -rm *.o x64/obj/* x64/bin/* x64_u/obj/* x64_u/bin/* x86/obj/* x86/bin/* x86_u/obj/* x86_u/bin/*
	CFLAGS=-g -Wall -Werror -pthread -lpthread
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
SRC64u	= 	x64_u/src
SRC32	=	x86/src
SRC32u	=	x86_u/src

OBJ64	=	x64/obj
OBJ64u	= 	x64_u/obj
OBJ32	=	x86/obj
OBJ32u	=	x86_u/obj


SRCS64=$(wildcard $(SRC64)/*.c)
OBJS64=$(patsubst $(SRC64)/%.c, $(OBJ64)/%.o, $(SRCS64))

SRCS64u=$(wildcard $(SRC64u)/*.c)
OBJS64u=$(patsubst $(SRC64u)/%.c, $(OBJ64u)/%.o, $(SRCS64u))

SRCS32=$(wildcard $(SRC32)/*.c)
OBJS32=$(patsubst $(SRC32)/%.c, $(OBJ32)/%.o, $(SRCS32))

SRCS32u=$(wildcard $(SRC32u)/*.c)
OBJS32u=$(patsubst $(SRC32u)/%.c, $(OBJ32u)/%.o, $(SRCS32u))

SHARE64		=	x64/$(SHARE)
SHARE64u	=	x64_u/$(SHARE)
SHARE32		=	x86/$(SHARE)
SHARE32u	=	x86_u/$(SHARE)


dir: 
	mkdir -p x64/obj
	mkdir -p x64_u/obj
	mkdir -p x86/obj
	mkdir -p x86_u/obj
	mkdir -p x86_u/bin
	mkdir -p x86/bin
	mkdir -p x64_u/bin
	mkdir -p x64/bin

all:$(dir) $(SHARE64) $(SHARE64u) $(SHARE32) $(SHARE32u)
	

$(SHARE64): $(OBJS64)
	$(CC) -fPIC -shared $(CFLAGS) $(OBJS64) -o $@

$(SHARE64u): $(OBJS64u)
	$(CC) -fPIC -shared $(CFLAGS) $(OBJS64u) -o $@

$(SHARE32): $(OBJS32)
	$(CC) -fPIC -shared $(CFLAGS) $(OBJS32) -o $@

$(SHARE32u): $(OBJS32u)
	$(CC) -fPIC -shared $(CFLAGS) $(OBJS32u) -o $@

$(OBJ64)/%.o: $(SRC64)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ64u)/%.o: $(SRC64u)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ32)/%.o: $(SRC32)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ32u)/%.o: $(SRC32u)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(CLEAN)
