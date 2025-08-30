/* This file has been created by EWA, and is part of task 4 & 6 on the exam for PG3401 2025*/
/* Original code credited to David Wheeler and Roger Needham */
/* Documentation: https://www.schneier.com/wp-content/uploads/2015/03/TEA-2.c */ 
#include "tea.h"

#define TEA_DELTA 0x9e3779b9
#define TEA_ROUNDS 32

void TEA_decrypt(uint32_t v[2], const uint32_t key[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = TEA_DELTA * TEA_ROUNDS;
    int round;
    for (round = 0; round < TEA_ROUNDS; round++) {
        v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
        v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
        sum -= TEA_DELTA;
    }
    v[0] = v0;
    v[1] = v1;
}


