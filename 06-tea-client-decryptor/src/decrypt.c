#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DELTA   0x9E3779B9U  /* konstant for krypteringsrotasjon */
#define ROUNDS  32           /* antall dekrypteringsrunder */

typedef unsigned int  UINT32;   /* 32-bits usignet heltall */
typedef unsigned char BYTE;     /* 8-bits usignet heltall */

/* dekrypterer pu32V[0..1] in-place med pu32Key[0..3] */
static void tea_decrypt(UINT32 pu32V[2], const UINT32 pu32Key[4])
{
    UINT32 ulY     = pu32V[0];
    UINT32 ulZ     = pu32V[1];
    UINT32 ulSum   = DELTA * ROUNDS;
    UINT32 ulDelta = DELTA;
    int    nRound;
    for (nRound = 0; nRound < ROUNDS; nRound++)
    {
        ulZ   -= ((ulY << 4) + pu32Key[2])
                ^ (ulY + ulSum)
                ^ ((ulY >> 5) + pu32Key[3]);
        ulY   -= ((ulZ << 4) + pu32Key[0])
                ^ (ulZ + ulSum)
                ^ ((ulZ >> 5) + pu32Key[1]);
        ulSum -= ulDelta;
    }
    pu32V[0] = ulY;
    pu32V[1] = ulZ;
}

/* sjekker at pucBuf[0..cbLen-1] kun inneholder 7-bit ASCII (<128) og returnerer 1 om OK, ellers 0 */
static int is_ascii_7(const BYTE *pucBuf, size_t cbLen)
{
    size_t i;
    for (i = 0; i < cbLen; i++)
    {
        if (pucBuf[i] & 0x80U)
            return 0;
    }
    return 1;
}

/* validerer og fjerner PKCS#5-padding (8-bytes blokk) og returnerer datalengde uten padding, eller 0 ved ugyldig padding */
static size_t remove_padding(BYTE *pucBuf, size_t cbLen)
{
    BYTE   byPad;
    size_t cbData, i;

    if (cbLen == 0)
        return 0;
    byPad = pucBuf[cbLen - 1];
    if (byPad == 0 || byPad > 8)
        return 0;
    cbData = cbLen - byPad;
    for (i = cbData; i < cbLen; i++)
    {
        if (pucBuf[i] != byPad)
            return 0;
    }
    return cbData;
}

int main(void)
{
    const char *pszInFile   = "body.bin";
    const char *pszOutFile  = "plaintext.txt";
    FILE       *pFileIn     = NULL;
    FILE       *pFileOut    = NULL;
    long        lFileLen    = 0;
    BYTE       *pEncBuf     = NULL;
    BYTE       *pDecBuf     = NULL;
    UINT32      aKey[4];
    int         bKeyFound   = 0;
    unsigned    nKeyByte    = 0;
    size_t      cbOffset    = 0;
    size_t      cbValidLen  = 0;
    size_t      cb;
    int         retval      = 1; /* 1=feil, settes til 0 ved suksess */

    /* Ã¥pner kryptert fil */
    pFileIn = fopen(pszInFile, "rb");
    if (!pFileIn)
    {
        perror("fopen body.bin");
        return retval;
    }

    /* finner filstÃ¸rrelse, mÃ¥ vÃ¦re >0 og mÃ¥ kunne deles pÃ¥ 8 */
    if (fseek(pFileIn, 0, SEEK_END) != 0)
    {
        perror("fseek");
        fclose(pFileIn);
        return retval;
    }
    lFileLen = ftell(pFileIn);
    if (fseek(pFileIn, 0, SEEK_SET) != 0)
    {
        perror("fseek");
        fclose(pFileIn);
        return retval;
    }
    if (lFileLen <= 0 || (lFileLen % 8) != 0)
    {
        fprintf(stderr, "ugyldig filstÃ¸rrelse: %ld\n", lFileLen);
        fclose(pFileIn);
        return retval;
    }

    /* leser ciphertext */
    pEncBuf = malloc((size_t)lFileLen);
    if (!pEncBuf)
    {
        perror("malloc");
        fclose(pFileIn);
        return retval;
    }
    cb = fread(pEncBuf, 1, (size_t)lFileLen, pFileIn);
    fclose(pFileIn);
    if (cb != (size_t)lFileLen)
    {
        perror("fread");
        free(pEncBuf);
        return retval;
    }

    /* allokerer buffer for dekryptert data */
    pDecBuf = malloc((size_t)lFileLen);
    if (!pDecBuf)
    {
        perror("malloc");
        free(pEncBuf);
        return retval;
    }

    /* brute-forcer alle 256 nÃ¸kler med samme byte */
    for (nKeyByte = 0; nKeyByte < 256; nKeyByte++)
    {
        size_t iIndex;
        /* gjenta byte i alle 4 ord */
        aKey[0] = (UINT32)nKeyByte
                | ((UINT32)nKeyByte <<  8)
                | ((UINT32)nKeyByte << 16)
                | ((UINT32)nKeyByte << 24);
        aKey[1] = aKey[0];
        aKey[2] = aKey[0];
        aKey[3] = aKey[0];

        /* dekrypterer hver 8-bytes blokk */
        for (cbOffset = 0; cbOffset < (size_t)lFileLen; cbOffset += 8)
        {
            UINT32 v[2];
            memcpy(v, pEncBuf + cbOffset, 8);
            tea_decrypt(v, aKey);
            memcpy(pDecBuf + cbOffset, v, 8);
        }

        /* sjekker 7-bit ASCII og padding */
        if (!is_ascii_7(pDecBuf, (size_t)lFileLen))
            continue;
        cbValidLen = remove_padding(pDecBuf, (size_t)lFileLen);
        if (cbValidLen == 0)
            continue;

        /* printer ut om riktig nÃ¸kkel ble funnet */
        printf("funnet nÃ¸kkelbyte: 0x%02X\n", nKeyByte);

        /* printer ut hele 128-bit nÃ¸kkelen */
        printf("full nÃ¸kkel (16 bytes):");
        for (iIndex = 0; iIndex < 16; iIndex++)
        {
            printf(" %02X", nKeyByte);
        }
        printf("\n");

        /* printer 4Ã—32-bits ord i hex */
        printf("som 4Ã—32-bits-ord: 0x%08lX 0x%08lX 0x%08lX 0x%08lX\n",
               (unsigned long)aKey[0],
               (unsigned long)aKey[1],
               (unsigned long)aKey[2],
               (unsigned long)aKey[3]);

        bKeyFound = 1;
        break;
    }

    if (!bKeyFound)
    {
        fprintf(stderr, "ingen gyldig nÃ¸kkel funnet\n");
        free(pEncBuf);
        free(pDecBuf);
        return retval;
    }

    /* skriver dekryptert data uten padding til fil */
    pFileOut = fopen(pszOutFile, "wb");
    if (!pFileOut)
    {
        perror("fopen plaintext.txt");
        free(pEncBuf);
        free(pDecBuf);
        return retval;
    }
    if (fwrite(pDecBuf, 1, cbValidLen, pFileOut) != cbValidLen)
    {
        perror("fwrite");
        fclose(pFileOut);
        free(pEncBuf);
        free(pDecBuf);
        return retval;
    }
    fclose(pFileOut);

    /* print ut dersom dekryperting ble gjort uten feil */
    printf("dekryptering vellykket, %s (lengde %lu byte)\n",
           pszOutFile, (unsigned long)cbValidLen);

    free(pEncBuf);
    free(pDecBuf);
    return 0;
}


