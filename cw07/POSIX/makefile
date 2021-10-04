CC = gcc

all: compile

compile:
	$(CC) -Wall main.c -o  main -lpthread -lrt
	$(CC) -Wall chef.c -o chef -lpthread -lrt
	$(CC) -Wall deliverer.c -o deliverer -lpthread -lrt

test:
	./main

clean:
	rm -f main chef deliverer
