#define ALPHA_SIZE 4

// helper function, given a letter, return its rank in the alphabet
int alpha_rank(char l);

// naive suffix sorting algorithm
int* suffix_sorting_0(char* str, int len);

// suffix sorting algorithm by manber and myers
int* suffix_sorting_1(char* str, int len);
