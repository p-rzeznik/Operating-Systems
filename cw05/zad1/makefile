CC = gcc


all: compile test clean

compile:
	$(CC) -Wall main.c -o main

test:
	./main list.txt
	
clean:
	rm -f main

