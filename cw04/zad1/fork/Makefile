
all: compile test clean

compile:
	gcc -Wall main.c -o main

test:
	./main handler
	./main ignore
	./main mask
	./main pending

clean:
	rm -f main


