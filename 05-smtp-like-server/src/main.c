#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ewpdef.h"
#include "server.h"

/* funksjoner fra server */
int  StartServer(unsigned short port);
void ServeClient(int clientSock, const char *serverID);

/* skriv bruksmelding og avslutt */
static void Usage(const char *prog)
{
    fprintf(stderr, "Bruk: %s -port <num> -id <ServerNavn>\n", prog);
    exit(1);
}

int main(int argc, char *argv[])
{
    unsigned short port = 0;
    char serverID[32] = "";
    int arg, listenSock;

    /* leser og validerer kommandolinje-argumenter */
    if (argc != 5) Usage(argv[0]);
    for (arg = 1; arg < argc; arg += 2) {
        if (strcmp(argv[arg], "-port") == 0) {
            port = (unsigned short)atoi(argv[arg+1]);
        }
        else if (strcmp(argv[arg], "-id") == 0) {
            strncpy(serverID, argv[arg+1], sizeof(serverID)-1);
            serverID[sizeof(serverID)-1] = '\0';
        }
        else {
            Usage(argv[0]);
        }
    }
    if (port == 0 || serverID[0] == '\0') Usage(argv[0]);

    /* starter server og lytter pÃ¥ loopback:port */
    listenSock = StartServer(port);
    printf("Server \"%s\" lytter pÃ¥ 127.0.0.1:%u\n",
           serverID, port);

    /* aksepter og hÃ¥ndter klienter */
    for (;;) {
        struct sockaddr_in cliAddr;
        socklen_t addrlen = sizeof(cliAddr);
        int clientSock = accept(listenSock,
                                (struct sockaddr*)&cliAddr,
                                &addrlen);
        if (clientSock < 0) {
            perror("accept");
            continue;
        }
        ServeClient(clientSock, serverID);
        close(clientSock);
    }

    /* aldri nÃ¥dd */
    close(listenSock);
    return 0;
}


