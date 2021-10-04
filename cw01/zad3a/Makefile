CC = gcc


all: static shared

static:
	$(CC) -c library.c -o library.o
	ar rcs liblibrary.a library.o
	$(CC) -Wall main.c -L. -llibrary -o main1


shared:
	$(CC) -fPIC -shared library.c -o liblibrary.so
	$(CC) -Wall main.c -L. -Wl,-rpath=. -llibrary -o main2



clean:
	rm -f *.o

test:
	./main1 1 jobList.txt
	./main2 1 jobList.txt
	make clean
