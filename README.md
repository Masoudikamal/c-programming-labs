# C Programming Labs

Et porteføljeprosjekt som samler flere små C-labber i et nøytralt, ryddig oppsett.  
Hver mappe er et selvstendig delprosjekt med egen Makefile og README.  
Fokus: tydelig kode, enkel kjøring, og korte beskrivelser av hva som løses (ikke eksamensformalia).

## Delprosjekter

- 02-file-metadata  
  Leser en tekstfil og produserer metadata (djb2-hash, bokstavfrekvens A-Z, filstørrelse/sum) og skriver binær utdata.

- 03-flight-scheduler  
  Liten konsollapp som modellerer en flyliste med dobbeltlenket liste for avganger og enkeltlenket liste for passasjerer.

- 04-threaded-bytecounter  
  To tråder (produsent/forbruker) med synkronisering. Leser fil i blokker og teller forekomst av byteverdier 0x00..0xFF.

- 05-smtp-like-server  
  Minimal TCP-server på 127.0.0.1:<port> som simulerer en enkel SMTP-lignende dialog og lagrer meldingsdata til fil.

- 06-tea-client-decryptor  
  To små verktøy: fetch henter en binær payload; decrypt dekrypterer med TEA (Tiny Encryption Algorithm).

## Struktur

    c-programming-labs/
      datasets/
        ewa_data.bin
        logs/ewa_log.txt
      02-file-metadata/
      03-flight-scheduler/
      04-threaded-bytecounter/
      05-smtp-like-server/
      06-tea-client-decryptor/
      README.md
      .gitignore

## Bygg og kjør (generelt)

Linux / macOS / WSL / MSYS2 (med gcc og make):

    cd <delprosjekt>
    make
    ./<binær>   # se README i delmappen for eksempelbruk

Windows uten kompilator:
- Hopp over bygg lokalt, eller installer en av følgende:
  - WSL + Ubuntu:
    
        wsl --install
        sudo apt update && sudo apt install -y build-essential make

  - MSYS2:
    
        pacman -Syu
        pacman -S mingw-w64-ucrt-x86_64-gcc make

## Lisens

MIT

## Emner (topics)

c, linux, sockets, threads, datastructures, parsing, encryption, lab
