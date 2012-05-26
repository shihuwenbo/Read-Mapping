CC = gcc
CCFLAG = -Wall -Werror -O3 -std=c99 -lm
SUFSORT_OBJ = sufsort.o misc.o suffix_sort.o
GENTEST_OBJ = misc.o gentest.o
BINARY = sufsort gentest convert fmidx map

all: $(BINARY)

map: search.o map.o misc.o
	$(CC) map.o search.o misc.o $(CCFLAG) -o map

map.o: map.c
	$(CC) $(CCFLAG) -c map.c

gentest: $(GENTEST_OBJ)
	$(CC) $(GENTEST_OBJ) $(CCFLAG) -o gentest

gentest.o: gentest.c
	$(CC) $(CCFLAG) -c gentest.c

sufsort: $(SUFSORT_OBJ)
	$(CC) $(SUFSORT_OBJ) $(CCFLAG) -o sufsort

convert: misc.o convert.o
	$(CC) $(CCFLAG) misc.o convert.o -o convert

convert.o: convert.c
	$(CC) $(CCFLAG) -c convert.c

fmidx: fmidx.o misc.o
	$(CC) $(CCFLAG) misc.o fmidx.o -o fmidx

fmidx.o: fmidx.c
	$(CC) $(CCFLAG) -c fmidx.c

sufsort.o: sufsort.c
	$(CC) $(CCFLAG) -c sufsort.c

suffix_sort.o: suffix_sort.c
	$(CC) $(CCFLAG) -c suffix_sort.c

misc.o: misc.c
	$(CC) $(CCFLAG) -c misc.c

search.o: search.c
	$(CC) $(CCFLAG) -c search.c

clean:
	rm -f *.o Makefile~ *.tar.gz

clobber:
	rm -f *.o $(BINARY) Makefile~ *.tar.gz

deltest:
	rm -f genome_*bit*

tarball:
	mkdir read_mapping
	cp *.h *.c Makefile read_mapping
	tar -pcvf read_mapping.tar.gz read_mapping/
	rm -rf read_mapping/
