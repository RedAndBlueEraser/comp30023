# 
# Makefile for COMP30023_2016_SM1 Project 1
# Written by Harry Wong
#

## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -Wall  


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		process-data-file-parser.c memory-management.c process-scheduler.c
OBJ =		process-data-file-parser.o memory-management.o process-scheduler.o
EXE = 		simulation

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ)


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ) 

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE) 

## Dependencies

process-data-file-parser.o:	process-data-file-parser.h
memory-management.o:		memory-management.h
process-scheduler.o:		process-scheduler.h memory-management.h process-data-file-parser.h
