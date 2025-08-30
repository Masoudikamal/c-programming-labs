# 02 - File Metadata

Dette delprosjektet leser en tekstfil og beregner en liten pakke metadata:
- djb2-hash (4 byte) av hele innholdet
- Bokstavfrekvens for A–Z (case-insensitiv)
- Filstørrelse (antall bytes)
- Sumsum av bytes/tegn (enkel sanity-sjekk)
- Skriver resultatet til en binær utdatafil (filnavn/format styres av koden)

Hensikt:
- Vise enkel filbehandling i C (streaming I/O)
- Modulært design med separate .c/.h-filer
- Lage et kompakt «fingeravtrykk» av innholdet i en tekstfil

## Struktur

    02-file-metadata/
      src/
        main.c    # styrer flyt og argumenter
        hash.c    # djb2-implementasjon
        count.c   # teller frekvens A–Z
        sum.c     # summerer bytes/tegn
      include/
        hash.h
        count.h
        sum.h
      data/
        sample.txt  # eksempelinput
      Makefile       # bygger binæren: filemeta

## Bygg og kjør

Linux / macOS / WSL / MSYS2 (med gcc og make):
    
    make
    ./filemeta data/sample.txt

Windows uten gcc:
- Det er helt OK å hoppe over bygging nå. Du kan bygge senere via WSL eller MSYS2.

## Inndata og utdata

Input:
- En tekstfil (for eksempel data/sample.txt)

Utdata:
- En binær fil med beregnet metadata (navn og layout styres av din implementasjon).
- Noen varianter skriver også en kort rapport i konsollen (avhenger av main.c).

## Feilhåndtering (anbefalt)

- Sjekk at inputfilen finnes og kan åpnes.
- Ignorer ikke-alfabetiske tegn i frekvenstelling (teller kun A–Z).
- Lukk filpekere og sjekk returverdier fra fread/fwrite.

## Lisens

MIT
