CC = gcc

all: compile

compile:
	$(CC) -Wall server.c -o server
	$(CC) -Wall customer.c -o customer

cust:
	./customer

serv:
	./server

clean:
	rm -f server customer
