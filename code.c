#include "code.h"
#include "defines.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//This is the initializer for the data type Code. Sets the top to 0 and sets the array of bits to 0.
Code code_init(void) {
    Code c;
    c.top = 0;
    for (int i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = 0;
    }
    return c;
}

//This accessor function will return the size of the code by referencing the top of the code.
uint32_t code_size(Code *c) {
    return c->top;
}

//This accessor function will indicate whether or not the code is empty by checking if the top of the code is 0. Returns true if so and false otherwise.
bool code_empty(Code *c) {
    return c->top == 0;
}

//This accessor function will indicate whether or not the code is full by checking if the top of the code is ALPHABET. Returns true if so and false otherwise.
bool code_full(Code *c) {
    return c->top == ALPHABET;
}

//This manipulator function will set the bit to 1 if it is in range. Returns true for a success and false if it is out of range.
bool code_set_bit(Code *c, uint32_t i) {
    if (i > ALPHABET) {
        return false;
    }
    c->bits[i / 8] |= (0x1 << (i % 8));
    return true;
}

//This manipulator function will set the bit to 0 if it is in range. Returns true for a sucess and false if it is out of range.
bool code_clr_bit(Code *c, uint32_t i) {
    if (i > ALPHABET) {
        return false;
    }
    c->bits[i / 8] &= ~(0x1 << (i % 8));
    return true;
}

//This accessor function will get the bit from code at index i. Returns false if it is out of range or if the bit is 0. Returns true if bit is 1.
bool code_get_bit(Code *c, uint32_t i) {
    if (i > ALPHABET) {
        return false;
    }
    return (c->bits[i / 8] >> (i % 8)) & 0x1;
}

//This manipulator function will push a bit onto the code by using set and clr bit. It will set the bit at the top if the bit from the parameter is 1. If bit from the parameter is 0, clr the bit at the top. Afterwards, increment the top by 1 and return true. Returns false if the code is full.
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    if (bit == 1) {
        code_set_bit(c, c->top);
    } else {
        code_clr_bit(c, c->top);
    }
    c->top += 1;
    return true;
}

//This manipulator function will pop a bit from the code by using get bit and storing the value into the bit pointer that was passed as a parameter. Returns false if the code is empty and true if it successfully pops a bit.
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top -= 1;
    *bit = code_get_bit(c, c->top);
    return true;
}
