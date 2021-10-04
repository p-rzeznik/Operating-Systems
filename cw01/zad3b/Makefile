CC = gcc


all: static_O0 static_O1 static_Os shared_O0 shared_O1 shared_Os

static_O0:
	$(CC) -c library.c -o library.o
	ar rcs liblibrary.a library.o
	$(CC) -Wall main.c -L. -llibrary -o main -O0
	make test

static_O1:
	$(CC) -c library.c -o library.o
	ar rcs liblibrary.a library.o
	$(CC) -Wall main.c -L. -llibrary -o main -O1
	make test

static_Os:
	$(CC) -c library.c -o library.o
	ar rcs liblibrary.a library.o
	$(CC) -Wall main.c -L. -llibrary -o main -Os
	make test

shared_O0:
	$(CC) -fPIC -shared library.c -o liblibrary.so
	$(CC) -Wall main.c -L. -Wl,-rpath=. -llibrary -o main -O0
	make test

shared_O1:
	$(CC) -fPIC -shared library.c -o liblibrary.so
	$(CC) -Wall main.c -L. -Wl,-rpath=. -llibrary -o main -O1
	make test

shared_Os:
	$(CC) -fPIC -shared library.c -o liblibrary.so
	$(CC) -Wall main.c -L. -Wl,-rpath=. -llibrary -o main -Os
	make test
	


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main

test:
	./main 1 jobList.txt
	make clean
