CC = gcc

all: compile

compile:
	$(CC) -Wall server.c -o server -lpthread
	$(CC) -Wall client.c -o client -lpthread


test:
	./server 8888 sock_path

cl1_local:
	./client Ania local sock_path
cl2_local:
	./client Basia local sock_path
cl1_network:
	./client Kasia network 8888
cl2_network:
	./client Asia network 8888
clean:
	rm -f server client sock_path