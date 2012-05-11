CC=gcc
CCFLAG=-Wall -Werror -O3
OBJECTS=dna_encoder.o dna_decoder.o
BINARY=encoder decoder

all: $(BINARY)

encoder: dna_encoder.o
	$(CC) $(CCFLAG) dna_encoder.o -o encoder
decoder: dna_decoder.o
	$(CC) $(CCFLAG) dna_decoder.o -o decoder
dna_encoder.o: dna_encoder.c
	$(CC) $(CCFLAG) -c dna_encoder.c
dna_decoer.o: dna_decoder.c
	$(CC) $(CCFLAG) -c dna_decoder.c

clean:
	rm $(OBJECTS) $(BINARY)
