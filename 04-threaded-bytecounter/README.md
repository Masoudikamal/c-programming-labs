# 04 - Threaded Byte Counter

To traader (produsent/forbruker) med synkronisering:
- Produsent: leser en tekstfil i blokker
- Forbruker: teller forekomster av byte-verdier 0x00..0xFF
- Maalet er aa vise enkel parallellisering i C med tydelig ansvarsdeling og trygg deling av buffer

## Struktur

    04-threaded-bytecounter/
      src/
        threads.c   # traadlogikk, delt buffer, synk
        (dbj2.c / tea.c hvis brukt som hjaelp)
      include/
        threads.h
      data/
        hamlet.txt  # eksempeldata
      Makefile       # bygger binaeren: bytecounter

## Bygg og kjor

Linux / macOS / WSL / MSYS2 (med gcc og make):

    make
    ./bytecounter data/hamlet.txt

Windows uten gcc:
- Hopp over bygg naa; bygg senere i WSL eller MSYS2.

## Output (forventet)

- En tabell eller linjer som viser antall for hver byte 0..255
- Oppsummering (totalt antall leste bytes)
- Eventuelt tidsmaal (avhenger av implementasjon)

## Synkronisering (anbefalt praksis)

- Beskytt delt buffer med mutex
- Bruk betingelsesvariabler (cond) for aa signalisere "buffer full/ledig"
- Ha tydelig avslutningssignal: naa end-of-file -> produsent signaliserer ferdig, forbruker tømmer og terminerer

## Feilhandtering (anbefalt)

- Sjekk at inputfil finnes og kan aapnes
- Sjekk returverdier fra trådkall (pthread_create/join) hvis brukt
- Frigi all allokert ressurs ved avslutning

## Lisens

MIT
