CC = gcc
CCFLAG = -Wall -Werror -O3 -std=c99
OBJECTS = main.o suffix_sort.o timer.o
BINARY = suffix_sort

$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) $(CCFLAG) -o $(BINARY)

main.o: main.c
	$(CC) $(CCFLAG) -c main.c

suffix_sort.o: suffix_sort.c
	$(CC) $(CCFLAG) -c suffix_sort.c
timer.o: timer.c
	$(CC) $(CCFLAG) -c timer.c

clean:
	rm $(OBJECTS) $(BINARY)
