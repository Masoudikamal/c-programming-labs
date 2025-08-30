#ifndef TASK4_THREADS_H
#define TASK4_THREADS_H

#include <stddef.h>
#include <semaphore.h>

/* argumentstruktur som deles mellom tråd A (fil-lesing) og tråd B (hashing og kryptering) */
typedef struct {
    const char *filename;  /* inputfilens navn */
    sem_t semAtoB;         /* semafor fra A til B */
    sem_t semBAck;         /* semafor for tilbakemelding B->A */
    char buffer[4096];     /* midlertidig lesebuffer */
    size_t bytes;          /* antall byte siste lesing */
    int finished;          /* flag som settes når A er ferdig */
    uint8_t *data;         /* dynamisk buffer for samlet fil-data */
    size_t data_len;       /* løpende lengde av samlet data */
    size_t data_cap;       /* kapasitet i 'data' */
} ThreadArgs;

/* tråd-funksjoner */
void *threadA_func(void *arg);
void *threadB_func(void *arg);

#endif /* TASK4_THREADS_H */
