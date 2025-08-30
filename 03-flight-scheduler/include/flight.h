#ifndef TASK3_H
#define TASK3_H

/* struktur for Ã©n passasjer (enkel lenket liste) */
typedef struct PASSENGER_tag {
    int iSeatRow;               /* seterad (1, 2, 3..) */
    char cSeatLetter;           /* setebokstav (aâ€“f) */
    char *pszName;              /* passasjerens navn */
    int iAge;                   /* passasjerens alder */
    struct PASSENGER_tag *pNext;/* neste passasjer pÃ¥ denne flyvningen */
} PASSENGER;

/* struktur for Ã©n flight (dobbel lenket liste) */
typedef struct FLIGHT_tag {
    char *pszFlightID;          /* FlightID, unikt */
    char *pszDestination;       /* destinasjon */
    int iSeats;                 /* totalt antall seter */
    int iDeparture;             /* avgangstid (f.eks 1345) */
    struct FLIGHT_tag *pPrev;   /* forrige flyvning i listen */
    struct FLIGHT_tag *pNext;   /* neste flyvning */
    PASSENGER *pPassengers;     /* hode pÃ¥ passasjerlisten */
} FLIGHT;

/* legger til flyvning bakerst
   returnerer 0 ved suksess
   !=0 ved feilsituasjon */
int AddFlight(FLIGHT **ppHead,
              const char *pszFlightID,
              const char *pszDestination,
              int iSeats,
              int iDeparture);

/* legger til passasjer pÃ¥ flyvning med gitt ID */
int AddPassenger(FLIGHT *pHead,
                 const char *pszFlightID,
                 const char *pszName,
                 int iAge,
                 const char *pszSeat);

/* finner flyvning 1-basert via indeks, eller NULL om ikke funnet */
FLIGHT *FindFlightByIndex(FLIGHT *pHead, int iIndex);

/* finner flyvningsindeks (1-basert) ved Ã¥ matche destinasjon, eller 0 om ikke funnet */
int FindFlightIndexByDestination(FLIGHT *pHead, const char *pszDestination);

/* sletter flyvninger (og alle tilhÃ¸rende passasjerer) */
int DeleteFlight(FLIGHT **ppHead, const char *pszFlightID);

/* endrer passasjers sete pÃ¥ gitt flyvning */
int ChangePassengerSeat(FLIGHT *pHead,
                        const char *pszFlightID,
                        const char *pszName,
                        const char *pszNewSeat);

/* skriver ut alle flyvning som en gitt passasjer er booket pÃ¥ */
void PrintPassengerFlights(FLIGHT *pHead, const char *pszName);

/* skriver ut passasjerer som er booket pÃ¥ mer enn Ã©n flyvning */
void PrintFrequentFliers(FLIGHT *pHead);

/* skriver ut Ã©n flyvning (alle detaljer inkl. passasjerliste) */
void PrintFlight(FLIGHT *pFlight, int iIndex);

/* printer alle flyvning med gitt destinasjon, case-insensitive */
void PrintFlightsByDestination(FLIGHT *pHead, const char *pszDestination);

/* frigir all minne for alle flyvninger og passasjerer */
void FreeAllFlights(FLIGHT **ppHead);

#endif /* TASK3_H */


