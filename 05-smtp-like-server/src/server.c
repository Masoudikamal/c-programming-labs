#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ewpdef.h"
#include "server.h"

int snprintf(char *str, size_t size, const char *format, ...);

#define BACKLOG       5
#define MAX_LINE_LEN 1024

/* leser nÃ¸yaktig len byte, returnerer <0 ved feil/lukking */
int RecvAll(int sock, void *buf, int len)
{
    int total = 0, got;
    char *p = buf;
    while (total < len) {
        got = recv(sock, p + total, len - total, 0);
        if (got <= 0) return got;
        total += got;
    }
    return total;
}
/* sender nÃ¸yaktig len byte, returnerer <0 ved feil */
int SendAll(int sock, const void *buf, int len)
{
    int sent = 0, wr;
    const char *p = buf;
    while (sent < len) {
        wr = send(sock, p + sent, len - sent, 0);
        if (wr <= 0) return wr;
        sent += wr;
    }
    return sent;
}
/* lager tidsstempel "DD MMM YYYY, HH:MM:SS" */
void MakeTimestamp(char *dst, int cap)
{
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    if (tm)
        strftime(dst, cap, "%d %b %Y, %H:%M:%S", tm);
    else
        strncpy(dst, "01 Jan 1970, 00:00:00", cap);
}

/* setter opp socket pÃ¥ loopback:<port> og begynner Ã¥ lytte */
int StartServer(unsigned short port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); exit(1); }

    /* gjenta adresse ved omstart */
    {
        int yes = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    }

    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind"); close(sock); exit(1);
        }
    }

    if (listen(sock, BACKLOG) < 0) {
        perror("listen"); close(sock); exit(1);
    }
    return sock;
}

/* kjÃ¸rer hele dialogen mot en klient */
void ServeClient(int sock, const char *serverID)
{
    char timestamp[32], sizeBuf[5], lineBuf[MAX_LINE_LEN];
    FILE *f;
    int i;

    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT acceptPkt;
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTHELO    heloPkt;
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO    srvHeloPkt;
    struct EWA_EXAM25_TASK5_PROTOCOL_MAILFROM      mailFromPkt;
    struct EWA_EXAM25_TASK5_PROTOCOL_RCPTTO        rcptToPkt;
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD dataCmdPkt;
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY   replyPkt;
    struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER    sizeHdr;

    /* 1 SERVERACCEPT 220 */
    memset(&acceptPkt,0,sizeof(acceptPkt));
    memcpy(acceptPkt.stHead.acMagicNumber,
           EWA_EXAM25_TASK5_PROTOCOL_MAGIC,3);
    sprintf(sizeBuf,"%04d",(int)sizeof(acceptPkt));
    memcpy(acceptPkt.stHead.acDataSize,sizeBuf,4);
    acceptPkt.stHead.acDelimeter[0]='|';
    memcpy(acceptPkt.acStatusCode,"220",3);
    acceptPkt.acHardSpace[0]=' ';
    MakeTimestamp(timestamp,sizeof(timestamp));
    snprintf(acceptPkt.acFormattedString,
             sizeof(acceptPkt.acFormattedString),
             "127.0.0.1 SMTP %s %s",
             serverID, timestamp);
    acceptPkt.acHardZero[0]='\0';
    SendAll(sock,&acceptPkt,sizeof(acceptPkt));

    /* 2 HELO til SERVERHELO (250) */
    if (RecvAll(sock,&heloPkt,sizeof(heloPkt))!=sizeof(heloPkt)) return;
    memset(&srvHeloPkt,0,sizeof(srvHeloPkt));
    memcpy(srvHeloPkt.stHead.acMagicNumber,
           EWA_EXAM25_TASK5_PROTOCOL_MAGIC,3);
    sprintf(sizeBuf,"%04d",(int)sizeof(srvHeloPkt));
    memcpy(srvHeloPkt.stHead.acDataSize,sizeBuf,4);
    srvHeloPkt.stHead.acDelimeter[0]='|';
    memcpy(srvHeloPkt.acStatusCode,"250",3);
    srvHeloPkt.acHardSpace[0]=' ';
    {
        char *dot = strchr(heloPkt.acFormattedString,'.');
        if (dot) {
            snprintf(srvHeloPkt.acFormattedString,
                     sizeof(srvHeloPkt.acFormattedString),
                     "%s Hello %.*s",
                     dot+1,
                     (int)(dot - heloPkt.acFormattedString),
                     heloPkt.acFormattedString);
        } else {
            strcpy(srvHeloPkt.acFormattedString,"Hello");
        }
    }
    srvHeloPkt.acHardZero[0]='\0';
    SendAll(sock,&srvHeloPkt,sizeof(srvHeloPkt));

    /* 3 MAIL FROM - 250 */
    if (RecvAll(sock,&mailFromPkt,sizeof(mailFromPkt))
        != sizeof(mailFromPkt)) return;
    memset(&replyPkt,0,sizeof(replyPkt));
    memcpy(replyPkt.stHead.acMagicNumber,
           EWA_EXAM25_TASK5_PROTOCOL_MAGIC,3);
    sprintf(sizeBuf,"%04d",(int)sizeof(replyPkt));
    memcpy(replyPkt.stHead.acDataSize,sizeBuf,4);
    replyPkt.stHead.acDelimeter[0]='|';
    memcpy(replyPkt.acStatusCode,"250",3);
    replyPkt.acHardSpace[0]=' ';
    strcpy(replyPkt.acFormattedString,"Sender address ok");
    replyPkt.acHardZero[0]='\0';
    SendAll(sock,&replyPkt,sizeof(replyPkt));

    /* 4 RCPT TO - 250 */
    if (RecvAll(sock,&rcptToPkt,sizeof(rcptToPkt))
        != sizeof(rcptToPkt)) return;
    SendAll(sock,&replyPkt,sizeof(replyPkt));

    /* 5 DATA:<filnavn> - 354(eller 501) */
    if (RecvAll(sock,&dataCmdPkt,sizeof(dataCmdPkt))
        != sizeof(dataCmdPkt)) return;
    {
        int len = strlen(dataCmdPkt.acFormattedString);
        for (i=0; i<len; i++) {
            unsigned char c = dataCmdPkt.acFormattedString[i];
            if (!isalnum(c) && c!='.'&&c!='_'&&c!='-') break;
        }
        memset(&replyPkt,0,sizeof(replyPkt));
        memcpy(replyPkt.stHead.acMagicNumber,
               EWA_EXAM25_TASK5_PROTOCOL_MAGIC,3);
        sprintf(sizeBuf,"%04d",(int)sizeof(replyPkt));
        memcpy(replyPkt.stHead.acDataSize,sizeBuf,4);
        replyPkt.stHead.acDelimeter[0]='|';
        if (len==0 || i<len) {
            memcpy(replyPkt.acStatusCode,"501",3);
            replyPkt.acHardSpace[0]=' ';
            strcpy(replyPkt.acFormattedString,"invalid filename");
            replyPkt.acHardZero[0]='\0';
            SendAll(sock,&replyPkt,sizeof(replyPkt));
            return;
        }
        memcpy(replyPkt.acStatusCode,"354",3);
        replyPkt.acHardSpace[0]=' ';
        strcpy(replyPkt.acFormattedString,"Ready for message");
        replyPkt.acHardZero[0]='\0';
        SendAll(sock,&replyPkt,sizeof(replyPkt));
    }

    /* 6 leser meldingslinjer til EOF markÃ¸r = "." */
    f = fopen(dataCmdPkt.acFormattedString,"wb");
    if (!f) return;
    for (;;) {
        int idx=0;
        char ch;
        do {
            if (recv(sock,&ch,1,0)<=0) { fclose(f); return; }
            if (idx<MAX_LINE_LEN-1) lineBuf[idx++]=ch;
        } while(ch!='\n');
        lineBuf[idx]='\0';
        if ((idx==2 && lineBuf[0]=='.'&&lineBuf[1]=='\n') ||
            (idx==3 && lineBuf[0]=='.'&&lineBuf[1]=='\r'&&lineBuf[2]=='\n'))
            break;
        fwrite(lineBuf,1,idx,f);
    }
    fclose(f);

    /* 7 250 Message received */
    memset(&replyPkt,0,sizeof(replyPkt));
    memcpy(replyPkt.stHead.acMagicNumber,
           EWA_EXAM25_TASK5_PROTOCOL_MAGIC,3);
    sprintf(sizeBuf,"%04d",(int)sizeof(replyPkt));
    memcpy(replyPkt.stHead.acDataSize,sizeBuf,4);
    replyPkt.stHead.acDelimeter[0]='|';
    memcpy(replyPkt.acStatusCode,"250",3);
    replyPkt.acHardSpace[0]=' ';
    strcpy(replyPkt.acFormattedString,"Message received");
    replyPkt.acHardZero[0]='\0';
    SendAll(sock,&replyPkt,sizeof(replyPkt));

    /* 8 QUIT - 221 */
    RecvAll(sock,&sizeHdr,sizeof(sizeHdr));
    memset(&replyPkt,0,sizeof(replyPkt));
    memcpy(replyPkt.stHead.acMagicNumber,
           EWA_EXAM25_TASK5_PROTOCOL_MAGIC,3);
    sprintf(sizeBuf,"%04d",(int)sizeof(replyPkt));
    memcpy(replyPkt.stHead.acDataSize,sizeBuf,4);
    replyPkt.stHead.acDelimeter[0]='|';
    memcpy(replyPkt.acStatusCode,"221",3);
    replyPkt.acHardSpace[0]=' ';
    strcpy(replyPkt.acFormattedString,"Closing connection");
    replyPkt.acHardZero[0]='\0';
    SendAll(sock,&replyPkt,sizeof(replyPkt));
}


