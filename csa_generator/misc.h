#define GENOME_SIZE 3000000000u
#define ENCODE_SIZE_2BIT 2
#define ENCODE_SIZE_3BIT 3
#define BYTE_SIZE 8
#define SHORT_SIZE (2 * BYTE_SIZE)

// star timer
void start_timer(double* time);

// stop timer
void stop_timer(double* time);

// generate random number between low and high
int randnum(int low, int high);

// get the pos-th bit in the array bit_vector
int get_bit(char* bit_vector, int pos);

// set the pos-th bit in the bit_vector to 1
void set_bit(char* bit_vector, int pos);

// clear the pos-th bit in the bit_vector to 0
void clear_bit(char* bit_vector, int pos);

// given a letter, return its rank in the alphabet
int alpha_rank(char l);

// get the bp at pos for 2 bit genome file
char get_bp_2bit(char* genome, unsigned int pos);

// get the bp at pos for 2 bit genome file
char get_bp_3bit(char* genome, unsigned int pos);

// write a bp at a position
void write_bp_2bit(char* genome, unsigned int pos, char val);

// write a bp at a position
void write_bp_3bit(char* genome, unsigned int pos, char val);

// open the file named file_name and read it into memory
void read_file(char* file_name, char** genome);

// write memory to disk saved in file_name
void write_file(char* file_name, char* genome, int encode_size);
