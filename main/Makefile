CC = gcc
CCFLAG = -Wall -Werror -O3 -std=c99 -lm
SUFSORT_OBJ = sufsort.o misc.o suffix_sort.o
GENTEST_OBJ = misc.o gentest.o
BINARY = sufsort gentest resume

all: $(BINARY)

gentest: $(GENTEST_OBJ)
	$(CC) $(GENTEST_OBJ) $(CCFLAG) -o gentest

gentest.o: gentest.c
	$(CC) $(CCFLAG) -c gentest.c

sufsort: $(SUFSORT_OBJ)
	$(CC) $(SUFSORT_OBJ) $(CCFLAG) -o sufsort

resume: misc.o resume.o
	$(CC) misc.o resume.o $(CCFLAG) -o resume

resume.o: resume.c
	$(CC) $(CCFLAG) -c resume.c

sufsort.o: sufsort.c
	$(CC) $(CCFLAG) -c sufsort.c

suffix_sort.o: suffix_sort.c
	$(CC) $(CCFLAG) -c suffix_sort.c

misc.o: misc.c
	$(CC) $(CCFLAG) -c misc.c

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
