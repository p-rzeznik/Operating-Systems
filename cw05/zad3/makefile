CC = gcc


all: compile

compile:
	$(CC) -Wall consumer.c -o consumer
	$(CC) -Wall producer.c -o producer
	$(CC) -Wall main.c -o main

fifo:
	mkfifo fifo

cons:
	./consumer fifo output.txt 5

prodA:	
	./producer fifo 3 A.txt 5
prodB:
	./producer fifo 1 B.txt 5
prodC:
	./producer fifo 4 C.txt 5
prodD:	
	./producer fifo 2 D.txt 5
prodE:	
	./producer fifo 5 E.txt 5

test:
	./main 


clean:
	rm -f consumer producer fifo main tmp.txt

