# -*- MakeFile -*-

CC=gcc
CFLAGS=-g -Wall -Werror
SRC=src
OBJ=obj
BINDIR=bin
BINEXE=$(BINDIR)/main
BINSO=$(BINDIR)/main.so
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

WDELFLAGS= /s /q /f
WBIN=.\bin\*
WOBJ=.\obj\*

SUBMITNAME=project.zip

all:$(BINEXE)

$(BINEXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

share: bin/comInterface.so

bin/comInterface.so: $(OBJS)
	$(CC) -fPIC -shared $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

shareW: bin/comInterface.dll

bin/comInterface.dll: $(OBJS)
	$(CC) -fPIC -shared $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(WDELFLAGS) $(WBIN) $(WOBJ)
