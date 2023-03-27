#include "code.h"
#include "endian.h"
#include "io.h"
#include "trie.h"

#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
                   "   Compresses files using the LZ78 compression algorithm.\n"
                   "   Compressed files are decompressed with the corresponding decoder.\n\n"

                   "USAGE\n"
                   "   ./encode [-vh] [-i input] [-o output]\n\n"
                   "OPTIONS\n"
                   "   -v             Display compression statistics\n"
                   "   -i input       Specify input to compress (stdin by default)\n"
                   "   -o output      Specify output of compressed input (stdout by default)\n"
                   "   -h             Display program help and usage\n");
            return 0;

        default: //default
            printf("SYNOPSIS\n"
                   "   Compresses files using the LZ78 compression algorithm.\n"
                   "   Compressed files are decompressed with the corresponding decoder.\n\n"

                   "USAGE\n"
                   "   ./encode [-vh] [-i input] [-o output]\n\n"
                   "OPTIONS\n"
                   "   -v             Display compression statistics\n"
                   "   -i input       Specify input to compress (stdin by default)\n"
                   "   -o output      Specify output of compressed input (stdout by default)\n"
                   "   -h             Display program help and usage\n");
            return 1;
        }
    }

    struct stat mask;
    fstat(infile, &mask);

    FileHeader header = { 0, 0 };
    header.magic = MAGIC; // magic number
    header.protection = mask.st_mode; // protection bit mask

    fchmod(outfile, header.protection); // permissions for outfile

    write_header(outfile, &header); // write file header to outfile

    TrieNode *root = trie_create(); // create trie
    TrieNode *curr_node = root; // root node copy

    uint16_t next_code = START_CODE; // counter that starts at START_CODE

    TrieNode *prev_node = NULL;
    uint8_t prev_sym = 0;

    uint8_t curr_sym = 0;
    while (read_sym(infile, &curr_sym)) { // loop to read all symbols from infile
        TrieNode *next_node = trie_step(curr_node, curr_sym); // set next_node to trie_step
        if (next_node != NULL) { // if next_node is not null
            prev_node = curr_node;
            curr_node = next_node;
        } else {
            // write pair of bit length of next_code
            write_pair(outfile, curr_node->code, curr_sym, log2(next_code) + 1);
            curr_node->children[curr_sym] = trie_node_create(next_code); // new trie node
            curr_node = root;
            next_code++;
        }
        if (next_code == MAX_CODE) { // if next_code is MAX_CODE
            trie_reset(root); //reset to root node
            curr_node = root;
            next_code = START_CODE;
        }

        prev_sym = curr_sym;
    }

    if (curr_node != root) { // checks if curr_node points to root
        write_pair(outfile, prev_node->code, prev_sym, log2(next_code) + 1); // write pair
        next_code = (next_code + 1) % MAX_CODE; // stays in limit of max code
    }
    write_pair(outfile, STOP_CODE, 0, log2(next_code) + 1); // end of output
    flush_pairs(outfile);
    trie_delete(root);

    if (verbose == 1) { // verbose enabled
        printf("Compressed file size: %lu bytes\n", total_syms);
        printf("Uncompressed file size: %lu bytes\n", total_bits);
        printf(
            "Compression ratio: %.2f%%\n", 100 * (1.0 - ((float) total_syms / (float) total_bits)));
    }
    // close files
    close(infile);
    close(outfile);
    return 0;
}
