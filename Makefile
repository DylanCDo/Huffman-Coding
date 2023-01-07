CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = -lm

all: encode decode

encode: encode.o stack.o node.o pq.o code.o huffman.o io.o
	$(CC) -o encode encode.o stack.o node.o pq.o code.o huffman.o io.o $(LFLAGS)

decode: decode.o stack.o node.o pq.o code.o huffman.o io.o
	$(CC) -o decode decode.o stack.o node.o pq.o code.o huffman.o io.o $(LFLAGS)

encode.o:  encode.c stack.c stack.h node.c node.h pq.c pq.h code.c code.h huffman.c huffman.h io.c io.h defines.h header.h
	$(CC) $(CFLAGS) -c encode.c stack.c node.c pq.c code.c huffman.c io.c

decode.o:  decode.c stack.c stack.h node.c node.h pq.c pq.h code.c code.h huffman.c huffman.h io.c io.h defines.h header.h
	$(CC) $(CFLAGS) -c decode.c stack.c node.c pq.c code.c huffman.c io.c

clean:
	rm -f encode encode.o decode decode.o stack.o node.o pq.o code.o huffman.o io.o

format:
	clang-format -i -style=file *.[ch]
