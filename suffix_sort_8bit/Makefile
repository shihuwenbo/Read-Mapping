CC = gcc
CCFLAG = -Wall -Werror -O3 -std=c99
OBJECTS = main.o suffix_sort.o
BINARY = sort 

$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) $(CCFLAG) -o $(BINARY)

main.o: main.c
	$(CC) $(CCFLAG) -c main.c

suffix_sort.o: suffix_sort.c
	$(CC) $(CCFLAG) -c suffix_sort.c

clean:
	rm $(OBJECTS) $(BINARY)
