#ifndef TASK5_SERVER_H
#define TASK5_SERVER_H

/* starter opp TCPâ€server pÃ¥ loopback:<port> */
int StartServer(unsigned short port);
/* tar imot en klient og kjÃ¸rer nettverksdialogen */
void ServeClient(int sock, const char *serverID);

#endif /* TASK5_SERVER_H */


