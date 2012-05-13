#define ALPHA_SIZE 4

// helper function, given a letter, return its rank in the alphabet
int alpha_rank(char l);

// naive suffix sorting algorithm
int* ss_naive(char* str, int len);

// suffix sorting algorithm by manber and myers
int* ss_mm(char* str, int len);

// refined version of suffix_sorting_1
int* ss_mm_refined(char* str, int len);

// get the pos-th bit in the array bit_vector
int get_bit(char* bit_vector, int pos);

// set the pos-th bit in the bit_vector to 1
void set_bit(char* bit_vector, int pos);

// clear the pos-th bit in the bit_vector to 0
void clear_bit(char* bit_vector, int pos);
