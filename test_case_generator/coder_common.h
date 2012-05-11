/* Encoding Rule: A: 00 (0), T: 01 (1), C: 10 (2), G: 11 (3) */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <error.h>
#include <string.h>

const size_t GENOME_SIZE = 3000000000u;
const size_t ENCODE_SIZE = 2;
const size_t BYTE_SIZE = 8;
