#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define ACK "HOLA"
#define WKP "WKP"
#define FIN "GOODBYE"

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000

int server_handshake(int *to_client);
int client_handshake(int *to_server);

void server_fin(int from_client, int to_client);
void client_fin(int from_server, int to_server);

#endif
