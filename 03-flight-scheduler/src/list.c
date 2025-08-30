#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "flight.h"

/* definerer antall seter per rad i flyet */
#define SEATS_PER_ROW 6

/* case-insensitiv strcmp */
static int StrCaseCmp(const char *a, const char *b)
{
    while (*a && *b) {
        char ca = toupper((unsigned char)*a);
        char cb = toupper((unsigned char)*b);
        if (ca != cb)
            return ca - cb;
        ++a; ++b;
    }
    return toupper((unsigned char)*a) - toupper((unsigned char)*b);
}

/* gjÃ¸r alle bokstaver i strengen uppercase */
static void ToUpperStr(char *s)
{
    while (*s) {
        *s = toupper((unsigned char)*s);
        ++s;
    }
}

/* finner flight i listen basert pÃ¥ ID (case-insensitivt) */
static FLIGHT *FindFlightByID(FLIGHT *pHead, const char *pszFlightID)
{
    FLIGHT *f;
    for (f = pHead;  f;  f = f->pNext) {
        if (StrCaseCmp(f->pszFlightID, pszFlightID) == 0)
            return f;
    }
    return NULL;
}

/* skriver ut alle flights som har gitt destinasjon */
void PrintFlightsByDestination(FLIGHT *pHead, const char *pszDestination)
{
    int idx = 1;
    int found = 0;
    FLIGHT *f;
    /* gÃ¥r gjennom alle flights */
    for (f = pHead;  f;  f = f->pNext, ++idx) {
        if (StrCaseCmp(f->pszDestination, pszDestination) == 0) {
            /* viser ID i uppercase og indeks */
            ToUpperStr(f->pszFlightID);
            printf("Destinasjon funnet pÃ¥ flight nummer %d (ID=%s)\n",
                   idx, f->pszFlightID);
            found = 1;
        }
    }
    if (!found)
        printf("Ingen flight med destinasjon %s\n", pszDestination);
}

/* skriver ut detaljinformasjon om Ã©n flight */
void PrintFlight(FLIGHT *pFlight, int iIndex)
{
    PASSENGER *q;
    if (!pFlight) {
        printf("Ingen flight pÃ¥ indeks %d\n", iIndex);
        return;
    }
    /* flightâ€ID i uppercase */
    ToUpperStr(pFlight->pszFlightID);
    printf("Flight %d:\n", iIndex);
    printf("  ID          : %s\n", pFlight->pszFlightID);
    printf("  Destination : %s\n", pFlight->pszDestination);
    printf("  Seats       : %d\n", pFlight->iSeats);
    printf("  Departure   : %04d\n", pFlight->iDeparture);
    /* skriver ut alle passasjerer pÃ¥ flight */
    for (q = pFlight->pPassengers;  q;  q = q->pNext) {
        printf("    Seat %d%c: %s, Age %d\n",
               q->iSeatRow,
               q->cSeatLetter,
               q->pszName,
               q->iAge);
    }
}

/* lager en C89-versjon av strdup */
static char *StrDup(const char *pszSrc)
{
    char *dst;
    size_t len;
    if (!pszSrc) return NULL;
    len = strlen(pszSrc) + 1;
    dst = malloc(len);
    if (dst) memcpy(dst, pszSrc, len);
    return dst;
}

/* leser setestreng som "12A" og sett row/letter, returner 0 om OK */
static int ParseSeat(const char *pszSeat, int *piRow, char *pcLetter, int iTotalSeats)
{
    int row;
    char c;
    if (!pszSeat || !piRow || !pcLetter) return 1;
    if (sscanf(pszSeat, "%d%c", &row, &c) != 2) return 1;
    if (row <= 0) return 1;
    c = tolower((unsigned char)c);
    if (c < 'a' || c > 'a' + SEATS_PER_ROW - 1) return 1;
    /* sjekker innenfor totalt antall seter */
    if (((row - 1) * SEATS_PER_ROW + (c - 'a' + 1)) < 1 ||
        ((row - 1) * SEATS_PER_ROW + (c - 'a' + 1)) > iTotalSeats)
        return 1;
    *piRow = row;
    *pcLetter = c;
    return 0;
}

/* legger til ny flight bakerst i listen */
int AddFlight(FLIGHT **ppHead,
              const char *pszFlightID,
              const char *pszDestination,
              int iSeats,
              int iDeparture)
{
    FLIGHT *pNew, *p;
    if (!ppHead || !pszFlightID || !pszDestination) return 1;
    if (FindFlightByID(*ppHead, pszFlightID)) return 2;
    if (iSeats <= 0) return 3;
    /* oppretter nytt flightâ€objekt */
    pNew = malloc(sizeof *pNew);
    if (!pNew) return 4;
    pNew->pszFlightID    = StrDup(pszFlightID);
    pNew->pszDestination = StrDup(pszDestination);
    if (!pNew->pszFlightID || !pNew->pszDestination) {
        free(pNew->pszFlightID);
        free(pNew->pszDestination);
        free(pNew);
        return 5;
    }
    pNew->iSeats      = iSeats;
    pNew->iDeparture  = iDeparture;
    pNew->pPassengers = NULL;
    pNew->pNext       = NULL;
    /* setter inn bakerst */
    if (!*ppHead) {
        pNew->pPrev = NULL;
        *ppHead     = pNew;
    } else {
        p = *ppHead;
        while (p->pNext) p = p->pNext;
        p->pNext    = pNew;
        pNew->pPrev = p;
    }
    return 0;
}

/* legger til passasjer pÃ¥ angitt flight (i sortert rekkefÃ¸lge) */
int AddPassenger(FLIGHT *pHead,
                 const char *pszFlightID,
                 const char *pszName,
                 int iAge,
                 const char *pszSeat)
{
    FLIGHT    *f;
    PASSENGER *q, *pNew, **pp;
    int row, i, newIdx;
    char letter;
    if (!pHead || !pszFlightID || !pszName || !pszSeat) return 1;
    /* finner flight */
    f = FindFlightByID(pHead, pszFlightID);
    if (!f) return 2;
    /* unngÃ¥r duplikat navn */
    for (q = f->pPassengers;  q;  q = q->pNext) {
        if (StrCaseCmp(q->pszName, pszName) == 0)
            return 3;
    }
    /* parser sete */
    if (ParseSeat(pszSeat, &row, &letter, f->iSeats) != 0)
        return 4;
    /* unngÃ¥r duplikat sete */
    for (q = f->pPassengers;  q;  q = q->pNext) {
        if (q->iSeatRow == row && q->cSeatLetter == letter)
            return 5;
    }
    /* oppretter passasjerâ€node */
    pNew = malloc(sizeof *pNew);
    if (!pNew) return 6;
    pNew->iSeatRow    = row;
    pNew->cSeatLetter = letter;
    pNew->pszName     = StrDup(pszName);
    pNew->iAge        = iAge;
    if (!pNew->pszName) { free(pNew); return 7; }
    /* finn riktig innsettingspunkt i listen */
    newIdx = (row - 1) * SEATS_PER_ROW + (letter - 'a' + 1);
    pp = &f->pPassengers;
    while (*pp) {
        PASSENGER *cur = *pp;
        i = (cur->iSeatRow - 1) * SEATS_PER_ROW + (cur->cSeatLetter - 'a' + 1);
        if (newIdx < i) break;
        pp = &cur->pNext;
    }
    /* setter inn og returner */
    pNew->pNext = *pp;
    *pp         = pNew;
    return 0;
}

/* finner flight basert pÃ¥ indeks i listen */
FLIGHT *FindFlightByIndex(FLIGHT *pHead, int iIndex)
{
    int i = 1;
    while (pHead && i < iIndex) {
        pHead = pHead->pNext;
        ++i;
    }
    return (i == iIndex ? pHead : NULL);
}

/* sletter en flight (og alle passasjerer pÃ¥ den) */
int DeleteFlight(FLIGHT **ppHead, const char *pszFlightID)
{
    FLIGHT    *p;
    PASSENGER *r, *t;
    if (!ppHead || !pszFlightID) return 1;
    p = FindFlightByID(*ppHead, pszFlightID);
    if (!p) return 2;
    /* frigir alle passasjerer */
    r = p->pPassengers;
    while (r) {
        t = r->pNext;
        free(r->pszName);
        free(r);
        r = t;
    }
    /* fjerner flight fra lenket liste */
    if (p->pPrev) p->pPrev->pNext = p->pNext;
    else          *ppHead = p->pNext;
    if (p->pNext) p->pNext->pPrev = p->pPrev;
    /* frigir flightâ€node */
    free(p->pszFlightID);
    free(p->pszDestination);
    free(p);
    return 0;
}

/* endrer sete for en passasjer */
int ChangePassengerSeat(FLIGHT *pHead,
                        const char *pszFlightID,
                        const char *pszName,
                        const char *pszNewSeat)
{
    FLIGHT    *f;
    PASSENGER *prev, *cur, *q;
    int row, newIdx, i;
    char letter;
    if (!pHead || !pszFlightID || !pszName || !pszNewSeat) return 1;
    /* finner flight */
    f = FindFlightByID(pHead, pszFlightID);
    if (!f) return 2;
    /* finner passasjer */
    prev = NULL;
    cur  = f->pPassengers;
    while (cur && StrCaseCmp(cur->pszName, pszName) != 0) {
        prev = cur;
        cur  = cur->pNext;
    }
    if (!cur) return 3;
    /* parser nytt sete */
    if (ParseSeat(pszNewSeat, &row, &letter, f->iSeats) != 0)
        return 4;
    /* unngÃ¥r at setet er opptatt */
    for (q = f->pPassengers;  q;  q = q->pNext) {
        if (q != cur && q->iSeatRow == row && q->cSeatLetter == letter)
            return 5;
    }
    /* fjerner cur fra lista */
    if (prev) prev->pNext = cur->pNext;
    else       f->pPassengers = cur->pNext;
    /* oppdaterer sete */
    cur->iSeatRow    = row;
    cur->cSeatLetter = letter;
    /* finner ny plass for cur */
    newIdx = (row - 1) * SEATS_PER_ROW + (letter - 'a' + 1);
    prev = NULL;
    q    = f->pPassengers;
    while (q) {
        i = (q->iSeatRow - 1) * SEATS_PER_ROW + (q->cSeatLetter - 'a' + 1);
        if (newIdx < i) break;
        prev = q;
        q    = q->pNext;
    }
    /* settrer cur inn pÃ¥ ny plass */
    if (prev) {
        cur->pNext   = prev->pNext;
        prev->pNext  = cur;
    } else {
        cur->pNext        = f->pPassengers;
        f->pPassengers   = cur;
    }
    return 0;
}

/* skriver ut hvilke flights en passasjer sitter pÃ¥ */
void PrintPassengerFlights(FLIGHT *pHead, const char *pszName)
{
    int found = 0;
    FLIGHT    *f;
    PASSENGER *q;
    for (f = pHead;  f;  f = f->pNext) {
        for (q = f->pPassengers;  q;  q = q->pNext) {
            if (StrCaseCmp(q->pszName, pszName) == 0) {
                printf("Passasjer %s er pÃ¥ flyvning %s\n",
                       pszName, f->pszFlightID);
                found = 1;
            }
        }
    }
    if (!found)
        printf("Passasjer %s er ikke booket pÃ¥ noe flyreise\n", pszName);
}

/* hjelpe-struktur for Ã¥ telle antall flyvninger per passasjer */
typedef struct FLIER {
    char      *pszName;
    int        iCount;
    struct FLIER *pNext;
} FLIER;

/* skriver ut passasjerer som flyr flere ganger */
void PrintFrequentFliers(FLIGHT *pHead)
{
    FLIER      *pList = NULL;
    FLIGHT     *f;
    PASSENGER  *q;
    FLIER      *e, *tmp;

    /* bygger opp en liste over navn + antall forekomster */
    for (f = pHead;  f;  f = f->pNext) {
        for (q = f->pPassengers;  q;  q = q->pNext) {
            e = pList;
            while (e) {
                if (StrCaseCmp(e->pszName, q->pszName) == 0) {
                    e->iCount++;
                    break;
                }
                e = e->pNext;
            }
            if (!e) {
                e = malloc(sizeof *e);
                e->pszName = StrDup(q->pszName);
                e->iCount  = 1;
                e->pNext   = pList;
                pList      = e;
            }
        }
    }

    /* skriver ut de som har vÃ¦rt pÃ¥ mer enn Ã©n flight */
    e = pList;
    while (e) {
        if (e->iCount > 1)
            printf("Hyppig flyver: %s er pÃ¥ %d flyvninger\n",
                   e->pszName, e->iCount);
        tmp = e->pNext;
        free(e->pszName);
        free(e);
        e = tmp;
    }
}

/* frigjÃ¸r alle flights og passasjerer */
void FreeAllFlights(FLIGHT **ppHead)
{
    while (*ppHead) {
        DeleteFlight(ppHead, (*ppHead)->pszFlightID);
    }
}


