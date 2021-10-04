CC = gcc

all: compile

compile:
	$(CC) -Wall server.c -o server -lrt
	$(CC) -Wall customer.c -o customer -lrt

cust:
	./customer

serv:
	./server

clean:
	rm -f server customer
