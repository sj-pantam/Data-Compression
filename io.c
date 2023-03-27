#include "io.h"
#include "code.h"
#include "endian.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

uint64_t total_syms = 0;
uint64_t total_bits = 0;

// reads and writes all in 4B blocks
static uint8_t bp[BLOCK] = { 0 }; // stores binary pairs
static int bp_index = 0; // index for bp
static uint8_t chars[BLOCK] = { 0 }; // stores characters
static int c_index = 0; // index for characters

// A wrapper function to read, read(), until all is read specified by nbytes or no more to read
int read_bytes(int infile, uint8_t *buf, int to_read) {
    int read_bytes = 0; // keeps track of # of bytes read
    while (to_read != read_bytes) { // iterates until all bytes read
        uint32_t bytes = read(infile, buf + read_bytes, to_read); // reads block of bytes
        if (bytes == 0) { // breaks out of loop when nothing is left to read
            break;
        }
        read_bytes += bytes; // total bytes read
    }
    total_bits += read_bytes;
    return read_bytes; // return bytes read
}

int write_bytes(int outfile, uint8_t *buf, int to_write) {
    int written = 0; // keeps track of # of bytes written
    while (to_write != written) { // iterate until no more bytes to write
        uint32_t bytes = write(outfile, buf + written, to_write); // write block of bytes
        if (bytes == 0) {
            break;
        }
        written += bytes; // total bytes written
    }
    total_syms += written;
    return written; // return bytes written
}

void read_header(int infile, FileHeader *header) {
    read_bytes(
        infile, (uint8_t *) header, sizeof(FileHeader)); // reads in sizeof() bytes to the header
    // if byte order is not little_endian, endianness is swapped
    if (!little_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }

    assert(header->magic == 0xBAADBAAC); // check the magic number
}

void write_header(int outfile, FileHeader *header) {
    write_bytes(
        outfile, (uint8_t *) header, sizeof(FileHeader)); // writes in sizeof() bytes to the header
    // if byte order is not little_endian, endianness is swapped
    if (!little_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
}

bool read_sym(int infile, uint8_t *sym) {
    static int done = 0; // keeps track of how many bytes have been read.
    uint64_t fill_up_bytes = 0;
    if (c_index == 0) { // read a block of data if index is 0
        fill_up_bytes = read_bytes(infile, chars, BLOCK); // Read data from the input file
        // If less than BLOCK bytes, reached the end of the infile.
        if (fill_up_bytes < BLOCK) {
            done = fill_up_bytes + 1; // sets to fill_up_bytes + 1 so index equals done
        }
    }
    *sym = chars[c_index]; // set the symbol at index c_index
    c_index++; // increment

    if (c_index == BLOCK) { // end of block
        c_index = 0; // reset index
    }

    if (c_index == done) { // if all symbols read return false else true
        return false;
    } else {
        return true;
    }
}

void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    for (int i = 0; i < bitlen; i++) { // iterate through each bit
        if (((code >> i) & 1) == 1) { // if i in code is 1 then set bit
            bp[bp_index / 8] |= (1 << (bp_index % 8));
        } else {
            bp[bp_index / 8] &= ~(1 << (bp_index % 8)); // clear the bit
        }
        bp_index++;
        if (bp_index == 8 * BLOCK) { // if index is at the end (8 * BLOCK) or full
            write_bytes(outfile, bp, BLOCK); // write to outfile
            bp_index = 0; //reset index
            flush_pairs(outfile); // flush remaining pairs
        }
    }
    for (int i = 0; i < 8; i++) { // iterate though bits in symbol
        if (((sym >> i) & 1) == 1) { // if i in sym is 1, then set bit
            bp[bp_index / 8] |= (1 << (bp_index % 8));
        } else {
            bp[bp_index / 8] &= ~(1 << (bp_index % 8)); // clear the bit
        }
        bp_index++;
        if (bp_index == 8 * BLOCK) { // if index is at end or full
            write_bytes(outfile, bp, BLOCK); // write to outfile
            bp_index = 0; // reset index
            flush_pairs(outfile); // flush remaining pairs
        }
    }
}

void flush_pairs(int outfile) {
    uint32_t bytes_div;
    uint32_t bytes_not_div;

    if (bp_index % 8 == 0) { // if the number of bits currently in the buffer is divisible by 8
        bytes_div = bp_index / 8;
        bp_index = 0;
        write_bytes(outfile, bp,
            bytes_div); // writes the exact number of bytes that are currently in the buffer to the output file
    } else {
        bytes_not_div = (((bp_index - 1) / 8) + 1);
        bp_index = 0;
        write_bytes(outfile, bp, bytes_not_div);
    }
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    *code = 0; //reset
    for (int i = 0; i < bitlen; i++) { // iterate through bits
        if (bp_index == 0) { // if index is 0 then read bytes
            read_bytes(infile, bp, BLOCK);
        }
        // if the bit is 1 then set it
        if (((bp[bp_index / 8] >> (bp_index % 8)) & 1) == 1) {
            *code |= (1 << i);
        } else { // clear bit
            *code &= ~(1 << i);
        }

        bp_index++;

        if (bp_index == 8 * BLOCK) { // if index at the end or full
            flush_pairs(infile); // flush remaining pairs
            bp_index = 0; //reset index
        }
    }
    *sym = 0; //reset
    for (int i = 0; i < 8; i++) { // iterate through (sym) bits
        if (bp_index == 0) { // if index 0 then read bytes
            read_bytes(infile, bp, BLOCK);
        }
        if (((bp[bp_index / 8] >> (bp_index % 8)) & 1) == 1) { // if the bit is 1 then set it
            *sym |= (1 << i);
        } else { // clear bit
            *sym &= ~(1 << i);
        }
        bp_index++;
        if (bp_index == 8 * BLOCK) { // if index at the end or full
            flush_pairs(infile); // flush remaining pairs
            bp_index = 0; //reset index
        }
    }
    if (*code == STOP_CODE) { // if equal to STOP_CODE, end of file
        return false;
    } else {
        return true;
    }
}

void write_word(int outfile, Word *w) {
    for (uint32_t i = 0; i < w->len; i++) { //iterate through the symbols
        chars[c_index] = w->syms[i]; // add symbol to buffer
        c_index++;
        if (c_index == BLOCK) { // if buffer is full
            write_bytes(outfile, chars, BLOCK); // write pair to outfile
            c_index = 0; // reset index
        }
    }
}

void flush_words(int outfile) { // flushes to outfile
    if (c_index > 0) {
        write_bytes(outfile, chars, c_index);
    }
}