#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024

/* skriver ut brukerveiledning og avslutter */
static void Usage(const char *prog) {
    fprintf(stderr, "bruk: %s -server <ip> -port <port>\n", prog);
    exit(1);
}

int main(int argc, char **argv) {
    const char *pszServer = NULL;   /* ip-adresse til server */
    int port = 0;                   /* portnummer til server */
    int i;                          /* lÃ¸kkevariabel for argparser */
    int sock;                       /* socket-deskriptor */
    struct sockaddr_in addr;        /* adresse-struktur for tilkobling */
    char buf[BUFSIZE];              /* mottaksbuffer */
    int n;                          /* antall bytes lest/skrevet */
    char *hdr = NULL;               /* buffer for header+body til bestemt punkt */
    int hdr_len = 0;                /* hvor mange byte vi har lagret i hdr */
    int hdr_cap = 0;                /* kapasitet til hdr-buffer */
    int sep = -1;                   /* indeks for "\r\n\r\n" separator */
    FILE *fh, *fb;                  /* filpeker for header.txt og body.bin */

    /* sjekkr antall argumenter */
    if (argc != 5) Usage(argv[0]);

    /* parser kommando-linje-argumenter */
    for (i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-server") == 0) {
            pszServer = argv[i+1];
        } else if (strcmp(argv[i], "-port") == 0) {
            port = atoi(argv[i+1]);
        } else {
            Usage(argv[0]);
        }
    }
    /* validerer nÃ¸dvendige parametre */
    if (!pszServer || port <= 0) Usage(argv[0]);

    /* oppretter TCP socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    /* fyller inn adresse-informasjon */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons((unsigned short)port);
    addr.sin_addr.s_addr = inet_addr(pszServer);

    /* kobleer til server */
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    /* leser data inntil vi finner "\r\n\r\n" (header/body separator) */
    while ((n = recv(sock, buf, BUFSIZE, 0)) > 0) {
        /* utvid hdr-buffer ved behov */
        if (hdr_len + n > hdr_cap) {
            hdr_cap = (hdr_len + n) * 2;
            hdr = realloc(hdr, hdr_cap);
            if (!hdr) {
                perror("realloc");
                close(sock);
                return 1;
            }
        }
        /* kopierer mottatte bytes til hdr */
        memcpy(hdr + hdr_len, buf, n);
        hdr_len += n;

        /* finner separatoren fÃ¸rste gang */
        if (sep < 0) {
            int j;
            for (j = 0; j + 3 < hdr_len; j++) {
                if (hdr[j] == '\r' && hdr[j+1] == '\n' &&
                    hdr[j+2] == '\r' && hdr[j+3] == '\n') {
                    sep = j + 4;  /* posisjon rett etter separator */
                    break;
                }
            }
        }
        /* stopper lesing av header nÃ¥r separator funnet */
        if (sep >= 0) break;
    }

    /* sjekker om vi fant separator */
    if (sep < 0) {
        fprintf(stderr, "fant ikke header/body-separator\n");
        free(hdr);
        close(sock);
        return 1;
    }

    /* skriver ut header til fil */
    fh = fopen("header.txt", "wb");
    if (!fh) {
        perror("fopen header.txt");
        free(hdr);
        close(sock);
        return 1;
    }
    fwrite(hdr, 1, sep, fh);
    fclose(fh);

    /* Ã¥pner body.bin for skriving */
    fb = fopen("body.bin", "wb");
    if (!fb) {
        perror("fopen body.bin");
        free(hdr);
        close(sock);
        return 1;
    }
    /* skriver allerede mottatt body-del fra hdr-buffer */
    if (hdr_len > sep) {
        fwrite(hdr + sep, 1, hdr_len - sep, fb);
    }
    /* leser resten av body fra socket og skriv til fil */
    while ((n = recv(sock, buf, BUFSIZE, 0)) > 0) {
        fwrite(buf, 1, n, fb);
    }
    fclose(fb);

    /* frigjÃ¸r midlertidig hdr-buffer */
    free(hdr);
    close(sock);

    /* skriver ut nÃ¥r filene er blitt lagret*/
    printf("lagret header.txt og body.bin\n");

    return 0;
}


