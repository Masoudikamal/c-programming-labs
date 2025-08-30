\# 05 – SMTP-like TCP Server



Minimal TCP-server på 127.0.0.1:<port> som simulerer en SMTP-lignende dialog:

\- `HELO` → 250 + klient-IP

\- `MAIL FROM` / `RCPT TO` → 250

\- `DATA <filnavn>` → 354 (valider filnavn) og skriv innhold til fil (avslutt med `\\r\\n.\\r\\n`)

\- `QUIT` → 221 og lukk



\## Bygg og kjør

\### Linux / macOS / WSL / MSYS2

```bash

make

./maild -port 2525 -id Demo



