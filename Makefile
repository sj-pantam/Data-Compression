CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -gdwarf-4

all: encode decode

encode: encode.o trie.o word.o io.o
	$(CC) -o encode encode.o trie.o word.o io.o -lm

decode: decode.o trie.o word.o io.o
	$(CC) -o decode decode.o trie.o word.o io.o -lm

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

trie.o: trie.c
	$(CC) $(CFLAGS) -c trie.c

word.o: word.c
	$(CC) $(CFLAGS) -c word.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

clean:
	rm -rf encode decode encode.o decode.o trie.o word.o io.o 

format: 
	clang-format -i --style=file *.[ch]

