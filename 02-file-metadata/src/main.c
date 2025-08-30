#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "count.h"
#include "sum.h"

#define INPUT_FILE    "pgexam25_test.txt"
#define OUTPUT_FILE   "pgexam25_output.bin"
#define MAX_NAME_LEN  32

/* inneholder filnavn, stÃ¸rrelse, hash, sum av alle tegn, og antall forekomster av hver bokstav (Aâ€“Z) */
struct TASK2_FILE_METADATA {
    char          fileName[MAX_NAME_LEN];
    int           fileSize;
    unsigned char hash[4];
    int           sumChars;
    char          alphaCount[26];
};

int main(void)
{
    FILE *in, *out;
    struct TASK2_FILE_METADATA meta;
    unsigned int djb2;
    int i, rc;

    /* Ã¥pner input-filen i Â«read binaryÂ»-modus */
    in = fopen(INPUT_FILE, "rb");
    if (!in) {
        perror("Kunne ikke Ã¥pne input-fil");
        return 1;
    }

    /* nullstiller hele metadata-strukturen */
    memset(&meta, 0, sizeof(meta));
    /* lagre filnavnet (passe pÃ¥ null-terminering) */
    strncpy(meta.fileName, INPUT_FILE, MAX_NAME_LEN - 1);

    /* funksjonen setter meta.fileSize og meta.sumChars. */
    rc = Task2_SizeAndSumOfCharacters(in,
                                      &meta.fileSize,
                                      &meta.sumChars);
    if (rc) {
        fprintf(stderr, "Error: SizeAndSumOfCharacters feilet\n");
        fclose(in);
        return 1;
    }

    /* fÃ¥r en 32-bits verdi, som vi pakker i 4 bytes */
    rc = Task2_SimpleDjb2Hash(in, &djb2);
    if (rc) {
        fprintf(stderr, "Error: SimpleDjb2Hash feilet\n");
        fclose(in);
        return 1;
    }
    /* little-endian pakkingen - tar laveste byte fÃ¸rst */
    for (i = 0;  i < 4;  i++) {
        meta.hash[i] = (djb2 >> (8 * i)) & 0xFF;
    }

    /* teller antall bokstaver Aâ€“Z/aâ€“z, og resultat legges i meta.alphaCount */
    rc = Task2_CountEachCharacter(in, meta.alphaCount);
    if (rc) {
        fprintf(stderr, "Error: CountEachCharacter feilet\n");
        fclose(in);
        return 1;
    }

    /* ferdig med input-filen */
    fclose(in);

    /* Ã¥pner output-filen i Â«write binaryÂ»-modus */
    out = fopen(OUTPUT_FILE, "wb");
    if (!out) {
        perror("Kunne ikke Ã¥pne output-fil");
        return 1;
    }

    /* skriver hele metadata-strukturen direkte som rÃ¥ binÃ¦rdata */
    fwrite(&meta, sizeof(meta), 1, out);
    fclose(out);

    return 0;
}


