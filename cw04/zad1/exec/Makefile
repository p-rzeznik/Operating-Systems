
all: compile test clean

compile:
	gcc -Wall main.c -o main
	gcc -Wall exec.c -o exec

test:
	./main ignore
	./main mask
	./main pending

clean:
	rm -f main
	rm -f exec

