#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>

#define BUFFER_SIZE    4096

/* TEAâ€nÃ¸kkel (128 bit) */
#define TEA_KEY0 0x01234567UL
#define TEA_KEY1 0x89ABCDEFUL
#define TEA_KEY2 0xFEDCBA98UL
#define TEA_KEY3 0x76543210UL

/* part I - struktur delt mellom trÃ¥der */
typedef struct _THREADCTX {
    char         *pszFileName;            /* inputâ€fil fra argv */
    sem_t         semBufEmpty;            /* 1 = A kan skrive, 0 = vent */
    sem_t         semBufFull;             /* 1 = B kan lese, 0 = vent */
    unsigned char aucBuffer[BUFFER_SIZE]; /* delt binÃ¦râ€buffer */
    size_t        uiBytesRead;            /* antall byte A har fylt */
    int           iExitThreadA;           /* A ferdig? 1 = ja */
    /* part I telleâ€array (ikke brukt i part II) */
    unsigned long aulCounts[256];
} THREADCTX;

/* DJB2â€hash */
static int Task2_SimpleDjb2Hash(FILE *fFileDescriptor, int *piHash)
{
    int hash;
    int iCharacter;

    hash = 5381;
    rewind(fFileDescriptor);
    do {
        iCharacter = fgetc(fFileDescriptor);
        if (iCharacter == 0 || iCharacter == EOF) break;
        hash = ((hash << 5) + hash) + (char)iCharacter; /* hash*33 + c */
    } while (iCharacter != EOF);
    *piHash = hash;
    rewind(fFileDescriptor);
    return 0;
}

/* TEAâ€kryptering */
static void tea_encrypt(uint32_t v[2], const uint32_t k[4])
{
    uint32_t tmp[2];
    unsigned long y, z, sum, delta, a, b, c, d;
    int n;

    /* kopierer input-ordene til en lokal buffer */
    tmp[0] = v[0];
    tmp[1] = v[1];

    /* initialiserer lokale variabler */
    y     = tmp[0];
    z     = tmp[1];
    sum   = 0UL;
    delta = 0x9E3779B9UL;
    a     = k[0];
    b     = k[1];
    c     = k[2];
    d     = k[3];
    n     = 32;

    /* 32 runder med TEA */
    while (n-- > 0) {
        sum += delta;
        y   += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
        z   += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
    }

    /* pakker resultatet tilbake til tmp */
    tmp[0] = (uint32_t)y;
    tmp[1] = (uint32_t)z;

    /* skriver tilbake til buffer */
    v[0] = tmp[0];
    v[1] = tmp[1];
}

/* thread A - leser fil i biter og signaliserer thread B */
static void *ThreadA_ReadFile(void *pCtx)
{
    THREADCTX *pst = (THREADCTX *)pCtx;
    FILE      *fp;
    size_t     uiRead;

    fp = fopen(pst->pszFileName, "rb");
    if (fp == NULL) {
        perror("fopen");
        pst->iExitThreadA = 1;
        sem_post(&pst->semBufFull);
        return NULL;
    }
    for (;;) {
        sem_wait(&pst->semBufEmpty);
        uiRead = fread(pst->aucBuffer, 1, BUFFER_SIZE, fp);
        pst->uiBytesRead = uiRead;
        if (uiRead == 0) {
            pst->iExitThreadA = 1;
            sem_post(&pst->semBufFull);
            break;
        }
        sem_post(&pst->semBufFull);
    }
    fclose(fp);
    return NULL;
}

/* thread B - teller byteâ€forekomster */
static void *ThreadB_CountBytes(void *pCtx)
{
    THREADCTX *pst = (THREADCTX *)pCtx;
    size_t     uiIdx;

    for (uiIdx = 0; uiIdx < 256; ++uiIdx)
        pst->aulCounts[uiIdx] = 0UL;

    for (;;) {
        sem_wait(&pst->semBufFull);
        if (pst->iExitThreadA && pst->uiBytesRead == 0)
            break;
        for (uiIdx = 0; uiIdx < pst->uiBytesRead; ++uiIdx)
            pst->aulCounts[pst->aucBuffer[uiIdx]]++;
        sem_post(&pst->semBufEmpty);
    }

    for (uiIdx = 0; uiIdx < 256; ++uiIdx) {
        if (pst->aulCounts[uiIdx] != 0UL) {
            printf("byte 0x%02X forekom %lu ganger\n",
                   (unsigned int)uiIdx,
                   pst->aulCounts[uiIdx]);
        }
    }
    return NULL;
}

/* part II - hash, padding, kryptering og filskriving */
static void PartII_ProcessFile(const char *pszFileName)
{
    FILE          *fpIn;
    unsigned char *pBuf;
    size_t         uiLen, uiPad, uiTotal, ui;
    int            iHash;
    unsigned char *pPadded;
    uint32_t       teaKey[4] = {TEA_KEY0, TEA_KEY1, TEA_KEY2, TEA_KEY3};
    FILE          *fpHash, *fpEnc;

    fpIn = fopen(pszFileName, "rb");
    if (fpIn == NULL) {
        perror("fopen PartII");
        return;
    }

    /* DBJ2â€hash fra fil */
    Task2_SimpleDjb2Hash(fpIn, &iHash);
    fpHash = fopen("task4_pg2265.hash", "w");
    if (fpHash) {
        fprintf(fpHash, "%d\n", iHash);
        fclose(fpHash);
    } else {
        perror("fopen .hash");
    }

    /* laster inn hele filen i minne */
    fseek(fpIn, 0, SEEK_END);
    uiLen = ftell(fpIn);
    rewind(fpIn);

    pBuf = (unsigned char *)malloc(uiLen);
    if (pBuf == NULL) {
        perror("malloc PartII");
        fclose(fpIn);
        return;
    }
    fread(pBuf, 1, uiLen, fpIn);
    fclose(fpIn);

    /* PKCS5â€padding til 64 byte */
    uiPad   = 64 - (uiLen % 64);
    uiTotal = uiLen + uiPad;
    pPadded = (unsigned char *)malloc(uiTotal);
    if (pPadded == NULL) {
        perror("malloc padded");
        free(pBuf);
        return;
    }
    memcpy(pPadded, pBuf, uiLen);
    for (ui = uiLen; ui < uiTotal; ++ui)
        pPadded[ui] = (unsigned char)uiPad;

    /* TEA kryptering, 8 byte om gangen */
    for (ui = 0; ui < uiTotal; ui += 8) {
        uint32_t v[2];
        memcpy(v, pPadded + ui, 8);
        tea_encrypt(v, teaKey);
        memcpy(pPadded + ui, v, 8);
    }

    /* skriver ut kryptert data */
    fpEnc = fopen("task4_pg2265.enc", "wb");
    if (fpEnc) {
        fwrite(pPadded, 1, uiTotal, fpEnc);
        fclose(fpEnc);
    } else {
        perror("fopen .enc");
    }

    free(pPadded);
    free(pBuf);
}

int main(int argc, char *argv[])
{
    THREADCTX stCtx;
    pthread_t tA, tB;
    int       iRet;

    /* sjekker at filnavn er oppgitt */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* init lokal kontekst */
    memset(&stCtx, 0, sizeof(stCtx));
    stCtx.pszFileName = argv[1];

    /* init semaforer - empty=1 vil A kunne skrive, full=0 mÃ¥ B vente*/
    sem_init(&stCtx.semBufEmpty, 0, 1);
    sem_init(&stCtx.semBufFull,  0, 0);

    /* oppretter part I */
    iRet = pthread_create(&tA, NULL, ThreadA_ReadFile,   &stCtx);
    if (iRet) {
        fprintf(stderr, "kunne ikke opprette Thread A\n");
        return EXIT_FAILURE;
    }
    iRet = pthread_create(&tB, NULL, ThreadB_CountBytes, &stCtx);
    if (iRet) {
        fprintf(stderr, "kunne ikke opprette Thread B\n");
        return EXIT_FAILURE;
    }

    /* venter pÃ¥ trÃ¥dene i part 1 avslutter */
    pthread_join(tA, NULL);
    pthread_join(tB, NULL);

    /* kjÃ¸reer part 2 - hash, pad, krypter og filskriving */
    PartII_ProcessFile(stCtx.pszFileName);

    /* rydder opp semaforer */
    sem_destroy(&stCtx.semBufEmpty);
    sem_destroy(&stCtx.semBufFull);

    return EXIT_SUCCESS;
}


