# 06 - TEA Client + Decryptor

To små verktøy som demonstrerer enkel klientnettverk og kryptering i C:
- **fetch**: kobler mot 127.0.0.1:<port> og henter en binær payload fra en tjeneste.
- **decrypt**: dekrypterer payload med **TEA** (Tiny Encryption Algorithm). I denne demoen bruker vi en enkel nøkkelvariant (128-bit der alle bytes kan være like), slik at fokus ligger på flyten og implementasjonen.

## Struktur

    06-tea-client-decryptor/
      src/
        fetch.c     # henter binær data fra server
        decrypt.c   # TEA-dekryptering av payload
        tea.c       # TEA-primitiver (kjerne)
      include/
        tea.h
      data/
        header.txt   # eksempel-metadata (kan brukes i test)
        body.bin     # eksempel-payload (kryptert)
        plaintext.txt# eksempel på forventet klartekst
      Makefile        # bygger binærene: fetch og decrypt

## Bygg og kjør

Linux / macOS / WSL / MSYS2 (med gcc og make):
    
    make
    # Eksempelbruk:
    ./fetch   -server 127.0.0.1 -port 42420 -o data/body.bin
    ./decrypt -i data/body.bin   -o data/plaintext.txt

Windows uten gcc:
- Det er helt OK å hoppe over bygg nå. Bygg senere via WSL eller MSYS2.

## Forventet arbeidsflyt

1. Start/ha en enkel tjeneste lokalt som leverer en binær payload (evt. bruk vedlagt data/body.bin).
2. Kjør **fetch** for å hente payload:
    
        ./fetch -server 127.0.0.1 -port 42420 -o data/body.bin

3. Kjør **decrypt** for å dekryptere payload med TEA:
    
        ./decrypt -i data/body.bin -o data/plaintext.txt

4. Verifiser resultatet ved å sammenligne mot data/plaintext.txt (om du bruker eksempelfilene).

## Feilhåndtering (anbefalt)

- Valider argumenter (server, port, input/output-stier).
- Sjekk returverdier fra socket-kall og fil-I/O; lukk alltid håndtak/pekere i feilstier.
- Bruk faste blokkstørrelser/protokollfelt slik at du vet hvor mye som skal leses/skrives.
- For TEA: pass på endianhet og antall runder dersom du tester mot andre implementasjoner.

## Lisens

MIT
