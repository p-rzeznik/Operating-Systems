$(CC) = gcc

all: test

test:
	$(CC) main.c -Wall -lm -o main
	./main dane.txt

generate:
	$(CC) generator.c -o generator
	./generator 10000000
