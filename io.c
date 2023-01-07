#include "io.h"
#include "code.h"
#include "defines.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;
static uint8_t writebuf[BLOCK] = { 0 };
static uint64_t w_index = 0;
static uint8_t readbuf[BLOCK] = { 0 };
static uint64_t r_index = 0; //Tracks position in buffer.
static uint64_t end = -1; //Tracks the alst valid bit.

//This code was heavily inspired by Christian's Section. This function reads the bytes and returns the total amount read from the infile.
int read_bytes(int infile, uint8_t *buf, int nbytes) {

    uint64_t current = 0; //Current amount of bytes read.
    uint64_t total = 0; //Total amount of bytes read.

    if (nbytes == 0) {
        return nbytes;
    }
    while ((current = read(infile, buf + total, nbytes - total)) > 0) {
        total += current;
        if (total >= (uint64_t) nbytes) {
            break;
        }
    }
    bytes_read += total;
    return total;
}

//This code was heavily inspired by Christian's Section. This function writes the byets and returns the total amount of bytes written to the outfile.
int write_bytes(int outfile, uint8_t *buf, int nbytes) {

    uint64_t current = 0; //Current amount of bytes wrote.
    uint64_t total = 0; //Total amount of bytes wrote.

    if (nbytes == 0) {
        return nbytes;
    }

    while ((current = write(outfile, buf + total, nbytes - total)) > 0) {
        total += current;
        if (total >= (uint64_t) nbytes) {
            break;
        }
    }

    bytes_written += total; //Adds the amount of bytes written to the overall total.
    return total;
}

//This code is the spitting image of Eugene's Psuedo code he made in his section on Novemeber 2nd, 2021
bool read_bit(int infile, uint8_t *bit) {

    if (r_index == 0) {
        int bytes = read_bytes(infile, readbuf, BLOCK);
        if (bytes < BLOCK) {
            end = bytes;
        }
    }

    *bit = (readbuf[r_index / 8] >> r_index % 8) & 0x1;
    r_index += 1; //Increments the index.

    //If the index reaches the value of BLOCK (4096), resets the index to 0 to avoid being out of bounds.
    if (r_index == BLOCK) {
        r_index = 0;
        for (int i = 0; i < BLOCK; i++) {
            readbuf[i] = 0;
        }
    }

    return r_index != end - 1;
}

//This code is the spitting image of Eugene's Psuedo code he made in his section on Novemeber 2nd, 2021
void write_code(int outfile, Code *c) {
    for (uint32_t i = 0; i < code_size(c); i++) {
        uint8_t bit = code_get_bit(c, i);
        if (bit == 1) {
            writebuf[w_index / 8] |= (0x1 << (w_index % 8)); //Set the bit at w_index.
        }

        w_index += 1; //Increments the index.

        //If the buffer is full, then it will flush the code.
        if (w_index == BLOCK) {
            flush_codes(outfile);
            w_index = 0;
            for (int i = 0; i < BLOCK; i++) {
                writebuf[i] = 0;
            }
        }
    }
}

//Flushed the code and writes the bytes to the outfile.
void flush_codes(int outfile) {
    if (w_index > 0) {
        uint64_t temp = w_index;
        // Calculating the amount of bytes for nbytes in the write bytes function by calculating the ceiling of the the index divided by 8.
        w_index = w_index / 8;
        if (temp % 8 > 0) {
            w_index += 1;
        }
        write_bytes(outfile, writebuf, w_index);
    }
}
