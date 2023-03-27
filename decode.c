#include "code.h"
#include "endian.h"
#include "io.h"
#include "math.h"
#include "word.h"

#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

int main(int argc, char **argv) {
    int opt = 0;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
    int verbose = 0;
    while ((opt = getopt(argc, argv, OPTIONS))
           != -1) { // Switch statement to check if label is matched
        switch (opt) {
        case 'i': infile = open(optarg, O_RDONLY); break;
        case 'o': outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC); break;
        // verbose enabled
        case 'v': verbose = 1; break;
        case 'h': // help message
            printf("SYNOPSIS\n"
                   "   Decompresses files with the LZ78 decompression algorithm.\n"
                   "   Used with files compressed with the corresponding encoder.\n\n"

                   "USAGE\n"
                   "   ./decode [-vh] [-i input] [-o output]\n\n"
                   "OPTIONS\n"
                   "   -v             Display decompression statistics\n"
                   "   -i input       Specify input to decompress (stdin by default)\n"
                   "   -o output      Specify output of decompressed input (stdout by default)\n"
                   "   -h             Display program usage\n");
            return 0;

        default: //default
            printf("SYNOPSIS\n"
                   "   Decompresses files with the LZ78 decompression algorithm.\n"
                   "   Used with files compressed with the corresponding encoder.\n\n"

                   "USAGE\n"
                   "   ./decode [-vh] [-i input] [-o output]\n\n"
                   "OPTIONS\n"
                   "   -v             Display decompression statistics\n"
                   "   -i input       Specify input to decompress (stdin by default)\n"
                   "   -o output      Specify output of decompressed input (stdout by default)\n"
                   "   -h             Display program usage\n");
            return 1;
        }
    }

    FileHeader header = { 0, 0 };

    read_header(infile, &header); // read in file header

    if (header.magic != MAGIC) { // verify magic number
        return -1;
    }

    WordTable *wt = wt_create(); // create word table
    uint16_t curr_code = 0;
    uint16_t next_code = START_CODE;

    uint8_t curr_sym = 0;
    // loop to read all pairs in input file
    while (read_pair(infile, &curr_code, &curr_sym, log2(next_code) + 1)) {
        wt[next_code] = word_append_sym(wt[curr_code], curr_sym); // append read sym
        write_word(outfile, wt[next_code]); // write the word

        next_code++;
        if (next_code == MAX_CODE) { // if next_code equals MAX_CODE
            wt_reset(wt); // reset WordTable
            next_code = START_CODE;
        }
    }

    flush_words(outfile); // flush buffered words
    for (uint32_t i = 0; i < MAX_CODE; i++) { // delete wt and free memory
        if (wt[i] != NULL) {
            word_delete(wt[i]);
        }
    }
    free(wt);

    if (verbose == 1) { // verbose enabled
        printf("Compressed file size: %lu bytes\n", total_bits);
        printf("Uncompressed file size: %lu bytes\n", total_syms);
        printf(
            "Compression ratio: %.2f%%\n", 100 * (1.0 - ((float) total_bits / (float) total_syms)));
    }
    // close files
    close(infile);
    close(outfile);
    return 0;
}
