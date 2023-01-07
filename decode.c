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

int infile = STDIN_FILENO;
int outfile = STDOUT_FILENO;
Header header;
struct stat f_info;
uint8_t tree_dump[MAX_TREE_SIZE] = { 0 };
uint8_t bit;
uint8_t symbol;

void help(); //Declaration for the help function.

uint8_t decode(); //Declaration for the decode function.

int main(int argc, char **argv) {

    bool verbose_printing = false;

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

    //Read the header of the file to obtain information.
    read_bytes(infile, (uint8_t *) &header, sizeof(Header));

    //Passes the header permission to the outfile.
    fchmod(outfile, header.permissions);

    //Reads the ree dump and stores it into the tree dump buffer array.
    read_bytes(infile, tree_dump, header.tree_size);

    //Recreates the ree to decode the file.
    Node *root = rebuild_tree(header.tree_size, tree_dump);

    //For loop that decodes the rest of the infile and puts it to the outfile.
    for (uint64_t i = 0; i < header.file_size; i++) {
        symbol = decode(infile, root);
        write_bytes(outfile, &symbol, 1);
    }

    //If user wants statistics, this function will be called.
    if (verbose_printing) {
        printf("Compressed file size: %" PRIu64 " bytes\n", f_info.st_size);
        printf("Decompressed file size: %" PRIu64 " bytes\n", bytes_written);
        printf("Space saving: %.2f%%\n", (float) (f_info.st_size - bytes_written) / f_info.st_size);
    }

    //Free up memory.
    delete_tree(&root);
    close(infile);
    close(outfile);
}

//This is a function that decodes an individual symbol.
uint8_t decode(int infile, Node *root) {
    //If the node is a leaf, return its symbol.
    if (root != NULL) {
        if (root->left == NULL && root->right == NULL) {
            return root->symbol;
        } else {
            read_bit(infile, &bit);
            //If the left node of the root exists, go down that path.
            if (bit == 0) {
                return decode(infile, root->left);
            } else {
                return decode(infile, root->right);
            }
        }
    }
    return 1;
}

//This function prints out the help statement if the user specifies it in
//command line or if the user inputs a non-existing command-line case.
void help() {
    printf("SYNOPSIS\n");
    printf("  A Huffman decoder.\n");
    printf("  Decompresses a file using the Huffman coding algorithm.\n");
    printf("\n");
    printf("USAGE\n");
    printf("  ./decode [-h] [-i infile] [-o outfile]\n");
    printf("\n");
    printf("OPTIONS\n");
    printf("  -h             Program usage and help.\n");
    printf("  -v             Print compression statistics.\n");
    printf("  -i infile      Input file to decompress.\n");
    printf("  -o outfile     Output of decompressed data.\n");
}
