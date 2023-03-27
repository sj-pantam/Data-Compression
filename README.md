# Data Compression Tool

## Description

The goal of this project is to create my own data compression chrome extension utilizing the 
the Lempel-Ziv compression algorithm, which is a lossless data compression technique. The algorithm identifies repeated patterns and generates a dictionary of codes to represent those patterns. This approach increases the efficiency and yields high compression results for data containing repetitive patterns, while preserving all information without loss. This will be one of many compression algorithms I am implementing.

## Program Compilation (NOTE: This is a sample LZ78 Compression for now)

To compile/build the entire program, enter: make

## How to run this program

### Encode

Compresses files using the LZ78 compression algorithm

To run encode.c enter: (./encode [-vh] [-i input] [-o output]). 

The command line options:

- `-v` Display compression statistics

- `-i` Specify input to compress (stdin by default)

- `-o` Specify output of compressed input (stdout by default)

- `-h` Display program help and usage

### Decode

Decompresses files with the LZ78 decompression algorithm

To run decode.c enter: (./decode [-vh] [-i input] [-o output]). 

The command line options:

- `-v` Display decompression statistics

- `-i` Specify input to decompress (stdin by default)

- `-o` Specify output of decompressed input (stdout by default)

- `-h` Display program usage
