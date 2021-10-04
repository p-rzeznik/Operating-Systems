
all: compile test clean

compile:
	gcc -Wall siginfo.c -o siginfo
	gcc -Wall nodefer.c -o nodefer


test:
	./siginfo sigint
	./siginfo sigabrt
	./siginfo sigusr
	./nodefer

clean:
	rm -f siginfo nodefer


