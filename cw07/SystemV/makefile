CC = gcc

all: compile

compile:
	$(CC) -Wall main.c -o main
	$(CC) -Wall chef.c -o chef
	$(CC) -Wall deliverer.c -o deliverer

test:
	./main

clean:
	rm -f main chef deliverer
