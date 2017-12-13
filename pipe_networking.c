#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe. (from server to client)

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
	//The well known pipe the client will connect to
	printf("[SERVER] creating well known pipe\n");
	int wkp = mkfifo(WKP, 0644);
	wkp = open(WKP, 0644);
	
	//Whenever client connects, it will read the data that it sends thru
	char data[HANDSHAKE_BUFFER_SIZE];
	printf("[SERVER] awaiting connection\n");
	read(wkp, data, HANDSHAKE_BUFFER_SIZE);
	
	//Client has sent name of private FIFO, open it
	printf("[SERVER] got message from client: %s\n", data);
	*to_client = open(data, O_WRONLY);
	
	//Remove the upstream WKP
	remove(WKP);
	printf("[SERVER] removing well known pipe\n");
	
	//Connect to the private FIFO and send an acknowledgement
	//strcpy(data, ACK);
	printf("[SERVER] sending acknowledgement to client\n");
	write(*to_client, ACK, strlen(data));
	
	//Now, wait for client to send back the acknowledgement
	read(wkp, data, sizeof(data));
	
	//Check if ACK and data match
	if (!strcmp(data, ACK)) {
		printf("[SERVER] received acknowledgement back\n");
		printf("[SERVER] connection esatblished\n");
	}
	else {
		printf("[SERVER] error: did not receive acknowledgement\n");
	}
	
	return wkp;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
	//The private the server will connect to
	printf("[CLIENT] creating priv pipe\n");
	char name[] = "werf";
	int priv = mkfifo(name, 0644);
	
	printf("[CLIENT] sending priv name to server\n");
	*to_server = open(WKP, 0644);
	write(*to_server, name, HANDSHAKE_BUFFER_SIZE);
	
	//Whenever client connects, it will read the data that it sends thru
	char data[HANDSHAKE_BUFFER_SIZE];
	printf("[CLIENT] reading server response\n");
	read(priv, data, HANDSHAKE_BUFFER_SIZE);
	printf("[CLIENT] data: %s\n", data);
	
	if (!strcmp(data, ACK)) printf("[CLIENT] ACK accepted\n");
	write(*to_server, data, strlen(data));
	
	
	return priv;
}
