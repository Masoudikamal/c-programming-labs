#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flight.h"

/* maks bufferstÃ¸rrelser for input */
#define INBUF    64
#define SEATBUF   8

/* fjerner '\n' pÃ¥ slutten av en C-streng */
static void StripNewline(char *psz)
{
    size_t len = strlen(psz);
    if (len > 0 && psz[len-1] == '\n')
        psz[len-1] = '\0';
}

/* hjelpefunksjon som erstatter fgets + StripNewline + returÂ­verdi-sjekk */
static int SafeFgets(char *buf, size_t sz)
{
    if (!fgets(buf, sz, stdin))
        return 0;          /* EOF eller feil */
    StripNewline(buf);
    return 1;
}

static void PrintMenu(void)
{
    printf("Meny for flyreiser:\n"
           "  Tast 1: Legg til flight\n"
           "  Tast 2: Legg til passasjer\n"
           "  Tast 3: Skriv ut flight ved indeks\n"
           "  Tast 4: Finn flight etter destinasjon\n"
           "  Tast 5: Slett flight\n"
           "  Tast 6: Endre passasjers sete\n"
           "  Tast 7: Skriv ut passasjerens flights\n"
           "  Tast 8: Skriv ut hyppige passasjerer\n"
           "  Tast 9: Avslutt\n"
           "Velg (1-9): ");
}

int main(void)
{
    FLIGHT *pHead = NULL;
    char inbuf[INBUF];

    for (;;) {
        PrintMenu();
        if (!SafeFgets(inbuf, sizeof inbuf))
            break;

        switch (atoi(inbuf)) {
        case 1: {  /* legger til flyvning */
            char flightID[INBUF], dest[INBUF];
            int seats, dep, res;

            printf("Flight ID: ");
            if (!SafeFgets(flightID, sizeof flightID)) break;

            printf("Destinasjon: ");
            if (!SafeFgets(dest, sizeof dest)) break;

            printf("Antall seter: ");
            if (!SafeFgets(inbuf, sizeof inbuf)) break;
            seats = atoi(inbuf);

            printf("Avgangstid (TTMM): ");
            if (!SafeFgets(inbuf, sizeof inbuf)) break;
            dep = atoi(inbuf);

            res = AddFlight(&pHead, flightID, dest, seats, dep);
            if (res != 0)
                printf("En feil oppsto ved Ã¥ legge til flyvning (kode %d)\n", res);
            break;
        }
        case 2: {  /* legger til passasjer */
            char flightID[INBUF], name[INBUF], seat[SEATBUF];
            int age, res;

            printf("Flight ID: ");
            if (!SafeFgets(flightID, sizeof flightID)) break;

            printf("Setenummer A-F (f.eks: 1A): ");
            if (!SafeFgets(seat, sizeof seat)) break;

            printf("Navn: ");
            if (!SafeFgets(name, sizeof name)) break;

            printf("Alder: ");
            if (!SafeFgets(inbuf, sizeof inbuf)) break;
            age = atoi(inbuf);

            res = AddPassenger(pHead, flightID, name, age, seat);
            if (res != 0)
                printf("En feil oppsto ved Ã¥ legge til passasjer (kode %d)\n", res);
            break;
        }
        case 3: {  /* skriver ut flyvningsindeks */
            int idx;

            printf("Indeks: ");
            if (!SafeFgets(inbuf, sizeof inbuf)) break;
            idx = atoi(inbuf);

            PrintFlight(FindFlightByIndex(pHead, idx), idx);
            break;
        }
        case 4: {  /* finn flight etter destinasjon */
            char buf[INBUF];

            printf("Destinasjon: ");
            if (!SafeFgets(buf, sizeof buf)) break;

            PrintFlightsByDestination(pHead, buf);
            break;
        }
        case 5: {  /* sletter flyvning */
            char flightID[INBUF];
            int res;

            printf("Flight ID Ã¥ slette: ");
            if (!SafeFgets(flightID, sizeof flightID)) break;

            res = DeleteFlight(&pHead, flightID);
            if (res != 0)
                printf("En feil oppsto ved Ã¥ slette flyvning %s (kode %d)\n", flightID, res);
            break;
        }
        case 6: {  /* endring av passasjersete */
            char flightID[INBUF], name[INBUF], newseat[SEATBUF];
            int res;

            printf("Flight ID: ");
            if (!SafeFgets(flightID, sizeof flightID)) break;

            printf("Passasjerens navn: ");
            if (!SafeFgets(name, sizeof name)) break;

            printf("Nytt setenummer A-F (f.eks. 12B): ");
            if (!SafeFgets(newseat, sizeof newseat)) break;

            res = ChangePassengerSeat(pHead, flightID, name, newseat);
            if (res != 0)
                printf("En feil oppsto ved sete-endring (kode %d)\n", res);
            break;
        }
        case 7: { /* skriver ut passasjerens flyvninger */
            printf("Passasjerens navn: ");
            if (SafeFgets(inbuf, sizeof inbuf))
                PrintPassengerFlights(pHead, inbuf);
            break;
        }
        case 8:  /* skriver ut hyppige passasjerer */
            PrintFrequentFliers(pHead);
            break;
        case 9:  /* avslutter */
            FreeAllFlights(&pHead);
            return 0;
        default:
            printf("Ugyldig valg\n");
        }
        printf("\n");
    }

    FreeAllFlights(&pHead);
    return 0;
}


