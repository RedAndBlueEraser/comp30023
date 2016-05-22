#
# Makefile for COMP30023_2016_SM1 Project 2
# Written by Harry Wong
#

## CC = Compiler.
## CFLAGS = Compiler flags.
CC = gcc
CFLAGS = -Wall -pthread -lnsl

## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.
GLOBALSRC = message-helpers.c
SERVERSRC = print-helpers.c secret-code.c server.c resource-usage.c
CLIENTSRC = client.c
GLOBALOBJ = message-helpers.o
SERVEROBJ = print-helpers.o secret-code.o server.o resource-usage.o
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
message-helpers.o: message-helpers.h
print-helpers.o: print-helpers.h
secret-code.o: secret-code.h
resource-usage.o: resource-usage.h
server.o: game-protocol.h message-helpers.h print-helpers.h secret-code.h
client.o: game-protocol.h message-helpers.h
