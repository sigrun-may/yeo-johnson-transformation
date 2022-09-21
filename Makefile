# -*- MakeFile -*-

CC=gcc
CFLAGS=-g -Wall -Werror
SRC=src
OBJ=obj
BINDIR=bin
BIN=$(BINDIR)/main
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

WDELFLAGS= /s /q /f
WBIN=.\bin\*
WOBJ=.\obj\*

SUBMITNAME=project.zip

all:$(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(WDELFLAGS) $(WBIN) $(WOBJ)
