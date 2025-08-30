# 03 - Flight Scheduler (C, datastrukturer)

Denne lille konsollappen modellerer en flyliste:
- Dobbeltlenket liste for flyavganger (id, destinasjon, klokkeslett, seter)
- Enkeltlenket liste for passasjerer per fly (navn, alder, sete)
- Grunnfunksjoner: legg til, søk, oppdater, slett, og enkel utskrift

Målet er å vise bruk av dynamiske datastrukturer i C, enkel menylogikk og ryddig moduldeling mellom header- og kildefiler.

## Struktur

    03-flight-scheduler/
      src/
        main.c   # meny/flow
        list.c   # listeoperasjoner (fly/passasjer)
      include/
        flight.h # strukturer + prototyper
      Makefile    # bygger binæren: flights

## Bygg og kjør

Linux / macOS / WSL / MSYS2 (med gcc og make):
    
    make
    ./flights

Windows uten gcc:
- Hopp over bygg nå; bygg senere via WSL eller MSYS2.

## Bruk (eksempel)

- Registrer fly (id, destinasjon, klokkeslett, seter)
- Legg til passasjer på et fly (navn, alder, sete)
- Søk/oppdater/slett poster
- List ut fly og passasjerer

## Feilhåndtering (anbefalt)

- Valider input (id, seter, alder, setenummer)
- Sjekk pekere før dereferering (head/tail)
- Frigjør all dynamisk allokert hukommelse ved avslutning

## Lisens

MIT
