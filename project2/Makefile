#
# Makefile for COMP30023_2016_SM1 Project 2
# Written by Harry Wong
#

## CC = Compiler.
## CFLAGS = Compiler flags.
CC = gcc
CFLAGS = -g -fstack-protector-all -Wall -pthread -lnsl

## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.
GLOBALSRC =
SERVERSRC = print-helpers.c secret-code.c server.c
CLIENTSRC = client.c
GLOBALOBJ =
SERVEROBJ = print-helpers.o secret-code.o server.o
CLIENTOBJ = client.o
SERVEREXE = server
CLIENTEXE = client

# Make server and client executables.
m: $(SERVEREXE) $(CLIENTEXE)

## Server: Make server executable.
$(SERVEREXE): $(GLOBALOBJ) $(SERVEROBJ)
	$(CC) $(CFLAGS) -o $(SERVEREXE) $(GLOBALOBJ) $(SERVEROBJ)

## Client: Make client executable.
$(CLIENTEXE): $(GLOBALOBJ) $(CLIENTOBJ)
	$(CC) $(CFLAGS) -o $(CLIENTEXE) $(GLOBALOBJ) $(CLIENTOBJ)

## Clean: Remove object files and core dump files.
clean:
	/bin/rm $(GLOBALOBJ) $(SERVEROBJ) $(CLIENTOBJ)

## Clobber: Performs Clean and removes executable file.
clobber: clean
	/bin/rm $(SERVEREXE) $(CLIENTEXE)

## Dependencies
print-helpers.o: print-helpers.h
secret-code.o: secret-code.h
server.o: game-protocol.h print-helpers.h secret-code.h
client.o: game-protocol.h
