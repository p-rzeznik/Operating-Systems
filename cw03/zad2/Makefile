
all: compile test clean

compile:
	gcc -c library.c -o library.o
	ar rcs liblibrary.a library.o
	gcc -Wall main.c -L. -llibrary -o main

test:
	./main 3 file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt
	./main 3 A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt
	./main 3 A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt
	./main 10 file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt
	./main 10 A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt
	./main 10 A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt
	./main 20 file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt file1.txt:file2.txt
	./main 20 A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt A1000.txt:B1000.txt
	./main 20 A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt A10000.txt:B10000.txt
  


clean:
	rm -f *.o
	rm -f main
	rm -f *.a

