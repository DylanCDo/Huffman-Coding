#include "code.h"
#include "stack.h"
#include "node.h"
#include "pq.h"
#include "io.h"
#include "huffman.h"
#include "defines.h"
#include "header.h"

#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

bool verbose_printing = false;
uint64_t hist[ALPHABET] = { 0 };
uint64_t total;
uint8_t buf[BLOCK];
Code table[ALPHABET];
struct stat f_info;
int infile = STDIN_FILENO;
int outfile = STDOUT_FILENO;
uint16_t unique_symbols = 0;

void help(); //Declaration for the help function.

int main(int argc, char **argv) {

    int opt = 0;

    //This while loop is responsible for parsing through the command-lines given by a user.
    while ((opt = getopt(argc, argv, "hi:o:v")) != -1) {

        //This if statement is responsible for printing out the help statement if the user inputs an unknown command-line.
        if (opt == '?') {
            help();
            return -1;
        }

        //This are all the cases.
        switch (opt) {
        case 'h': help(); return -1;
        case 'i':
            infile = open(optarg, O_RDONLY | O_CREAT);
            if (infile < 0) {
                perror("open");
                return 1;
            }
            break;
        case 'o':
            outfile = open(optarg, O_WRONLY | O_TRUNC | O_CREAT);
            if (outfile < 0) {
                perror("open");
                return 1;
            }
            break;
        case 'v': verbose_printing = true; break;
        }
    }

    //Pulls information from the infile.
    fstat(infile, &f_info);

    fchmod(outfile, f_info.st_mode);

    //Creates the histogram
    while ((total = read_bytes(infile, buf, BLOCK)) > 0) {
        for (uint64_t i = 0; i < total; i++) {
            hist[buf[i]] += 1;
        }
    }

    total = 0;

    //Increment the 255th and 0th so the histogram has two elements.
    hist[0] += 1;
    hist[255] += 1;

    //This section of the code will find all the unique symbols.
    for (uint64_t i = 0; i < ALPHABET; i++) {
        if (hist[i] > 0) {
            unique_symbols += 1;
        }
    }

    //This section of code will build the tree.
    Node *tree = build_tree(hist);

    //This builds the code based on the tree build prior.
    build_codes(tree, table);

    //This is where the header is declared for the outfile.
    Header ih = { ih.magic = MAGIC, ih.permissions = f_info.st_mode,
        ih.tree_size = (3 * unique_symbols) - 1, ih.file_size = f_info.st_size };

    //This pushes the header information into the outfile.
    write_bytes(outfile, (uint8_t *) &ih, sizeof(Header));

    //This dumps the tree into the outfil
    dump_tree(outfile, tree);

    //This sets the pointer that reads the file back to the start.
    lseek(infile, 0, SEEK_SET);

    //This section will reiterate through the file again and designate a code to each symbol.
    while ((total = read_bytes(infile, buf, BLOCK)) != 0) {
        for (uint64_t i = 0; i < total; i++) {
            if (buf[i] != 'a') {
            }
            write_code(outfile, &table[buf[i]]);
        }
    }

    //This flushes any remaining code to the outfile.
    flush_codes(outfile);

    //This prints the statistics of the encoding process.
    if (verbose_printing) {
        printf("Uncompressed file size: %" PRIu64 " bytes\n", ih.file_size);
        printf("Compressed file size: %" PRIu64 " bytes\n", bytes_written);
        printf("Space saving: %.2f%%\n", (float) (ih.file_size - bytes_written) / ih.file_size);
    }

    //Last section of code will free up memory.
    delete_tree(&tree);
    close(infile);
    close(outfile);
}
//This function prints out the help statement if the user specifies it in
//command line or if the user inputs a non-existing command-line case.
void help() {
    printf("SYNOPSIS\n");
    printf("  A Huffman encoder.\n");
    printf("  Compresses a file using the Huffman coding algorithm.\n");
    printf("\n");
    printf("USAGE\n");
    printf("  ./encode [-h] [-i infile] [-o outfile]\n");
    printf("\n");
    printf("OPTIONS\n");
    printf("  -h             Program usage and help.\n");
    printf("  -v             Print compression statistics.\n");
    printf("  -i infile      Input file to compress.\n");
    printf("  -o outfile     Output of compressed data.\n");
}
