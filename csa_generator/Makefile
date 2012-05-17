CC = gcc
CCFLAG = -Wall -Werror -O3 -std=c99 -lm
SORT_OBJECTS = sort.o suffix_sort.o misc.o
CONVERT_OBJECTS = convert.o misc.o
CSA_OBJECTS = gen_csa.o suffix_sort.o misc.o csa.o
BINARY = sort convert gencsa

all: $(BINARY)

gencsa: $(CSA_OBJECTS)
	$(CC) $(CSA_OBJECTS) $(CCFLAG) -o gencsa

sort: $(SORT_OBJECTS)
	$(CC) $(SORT_OBJECTS) $(CCFLAG) -o sort

convert: $(CONVERT_OBJECTS)
	$(CC) $(CONVERT_OBJECTS) $(CCFLAG) -o convert

gen_csa.o: gen_csa.c
	$(CC) $(CCFLAG) -c gen_csa.c

csa.o: csa.c
	$(CC) $(CCFLAG) -c csa.c

sort.o: sort.c
	$(CC) $(CCFLAG) -c sort.c

convert.o: convert.c
	$(CC) $(CCFLAG) -c convert.c

suffix_sort.o: suffix_sort.c
	$(CC) $(CCFLAG) -c suffix_sort.c

misc.o: misc.c
	$(CC) $(CCFLAG) -c misc.c

clean:
	rm -f *.o Makefile~ *.tar.gz

clobber:
	rm -f *.o $(BINARY) Makefile~ *.tar.gz

tarball:
	mkdir read_mapping
	cp *.h *.c Makefile read_mapping
	tar -pcvf read_mapping.tar.gz read_mapping/
	rm -rf read_mapping/
