// given a file of short reads, search it in genome, store result in all_ans
// maximum result is limited by ans_size
// maximum mismatch is kerr
void search(char* bwt, unsigned int genome_size, char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ, unsigned int spsize, unsigned int* all_ans,
        unsigned int ans_size, int kerr);

// given a bwt and a partial occ array, find the actual occ value
unsigned int get_occ(char* bwt, unsigned int* occ, unsigned int genome_size,
        unsigned int sample_size, long long int pos, char alpha);


// k-mismatch search
unsigned int kmismatch(char* sr, char* bwt, unsigned int* sml,
        unsigned int* occ, unsigned int* psa, int kerr,
        unsigned int rs, unsigned int re, unsigned int sp, unsigned int ep,
        unsigned int* ans, unsigned int ans_cnt, unsigned int ans_size,
        unsigned int genome_size, unsigned int sample_size,
        const char *alphabet);
