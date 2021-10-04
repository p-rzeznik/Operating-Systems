
all: test clean

test:
	gcc -c library.c -o library.o
	ar rcs liblibrary.a library.o
	gcc -Wall main.c -L. -llibrary -o main
	./main 1 jobList2.txt

clean:
	rm -f *.o
	rm -f main
	rm -f *.a

