OBJS = bitcoin.o main_functions.o help_functions.o
SOURCE = bitcoin.c main_functions.c help_functions.c
HEADER = functions.h structs.h
OUT = bitcoin
CC=gcc
FLAGS= -g -c
CLEAR = clear_screen

all: $(CLEAR) $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

clear_screen:
	clear

bitcoin.o: bitcoin.c
	$(CC) $(FLAGS) bitcoin.c

main_functions.o: main_functions.c
	$(CC) $(FLAGS) main_functions.c

help_functions.o: help_functions.c
	$(CC) $(FLAGS) help_functions.c

clean:
	rm -f $(OBJS) $(OUT)
