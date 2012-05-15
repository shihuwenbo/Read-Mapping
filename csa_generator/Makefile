CC = gcc
CCFLAG = -Wall -Werror -O3 -std=c99
CSA_OBJECTS = csa.o suffix_sort.o misc.o
CONVERT_OBJECTS = convert.o misc.o
BINARY = gen_csa convert

all: $(BINARY)

gen_csa: $(CSA_OBJECTS)
	$(CC) $(CSA_OBJECTS) $(CCFLAG) -o gen_csa

convert: $(CONVERT_OBJECTS)
	$(CC) $(CONVERT_OBJECTS) $(CCFLAG) -o convert

csa.o: csa.c
	$(CC) $(CCFLAG) -c csa.c

convert.o: convert.c
	$(CC) $(CCFLAG) -c convert.c

suffix_sort.o: suffix_sort.c
	$(CC) $(CCFLAG) -c suffix_sort.c
misc.o: misc.c
	$(CC) $(CCFLAG) -c misc.c

clean:
	rm *.o $(BINARY)
