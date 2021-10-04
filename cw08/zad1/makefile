CC = gcc

all: compile

compile:
	$(CC) -Wall main.c -o main -lpthread

test:
	./main 1 numbers saturn.ascii.pgm N1_saturn.ascii.pgm
	./main 2 numbers saturn.ascii.pgm N2_saturn.ascii.pgm
	./main 4 numbers saturn.ascii.pgm N4_saturn.ascii.pgm
	./main 8 numbers saturn.ascii.pgm N8_saturn.ascii.pgm
	./main 16 numbers saturn.ascii.pgm N16_saturn.ascii.pgm
	./main 8 numbers casablanca.ascii.pgm N8_casablanca.ascii.pgm

	./main 1 block saturn.ascii.pgm B1_saturn.ascii.pgm
	./main 2 block saturn.ascii.pgm B2_saturn.ascii.pgm
	./main 4 block saturn.ascii.pgm B4_saturn.ascii.pgm
	./main 8 block saturn.ascii.pgm B8_saturn.ascii.pgm
	./main 16 block saturn.ascii.pgm B16_saturn.ascii.pgm
	./main 8 block casablanca.ascii.pgm B8_casablanca.ascii.pgm


clean:
	rm -f main 

clean_negatives:
	rm -f N1_saturn.ascii.pgm N2_saturn.ascii.pgm N4_saturn.ascii.pgm N8_saturn.ascii.pgm N16_saturn.ascii.pgm N8_casablanca.ascii.pgm
	rm -f B1_saturn.ascii.pgm B2_saturn.ascii.pgm B4_saturn.ascii.pgm B8_saturn.ascii.pgm B16_saturn.ascii.pgm B8_casablanca.ascii.pgm