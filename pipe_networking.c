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
	
	//Whenever client connects, it will read the data that it sends thru
	char data[HANDSHAKE_BUFFER_SIZE];
	printf("[SERVER] awaiting connection\n");
	wkp = open(WKP, O_RDONLY);
	read(wkp, data, HANDSHAKE_BUFFER_SIZE);
	
	//Client has sent name of private FIFO, open it
	printf("[SERVER] got message from client: %s\n", data);
	*to_client = open(data, O_WRONLY);
	
	//Remove the upstream WKP
	printf("[SERVER] removing well known pipe\n");
	remove(WKP);
	
	//Connect to the private FIFO and send an acknowledgement
	//strcpy(data, ACK);
	printf("[SERVER] sending acknowledgement to client\n");
	write(*to_client, ACK, strlen(ACK));
	
	//Now, wait for client to send back the acknowledgement
	read(wkp, data, sizeof(data));
	
	//Check if ACK and data match
	if (!strcmp(data, ACK)) {
		printf("[SERVER] received acknowledgement back\n");
		printf("[SERVER] connection established\n");
	}
	else {
		printf("[SERVER] error: did not receive acknowledgement\n");
		exit(1);
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
	//The private pipe the server will connect to
	printf("[CLIENT] creating priv pipe\n");
	char name[] = "werf";
	int priv = mkfifo(name, 0644);
	
	//send private pipe name to server
	printf("[CLIENT] sending priv name to server\n");
	*to_server = open(WKP, O_WRONLY);
	write(*to_server, name, HANDSHAKE_BUFFER_SIZE);
	
	//Read the server's ACK
	char data[HANDSHAKE_BUFFER_SIZE];
	printf("[CLIENT] reading server response\n");
	priv = open(name, O_RDONLY);
	data[read(priv, data, HANDSHAKE_BUFFER_SIZE)] = 0;
	printf("[CLIENT] data: %s\n", data);
	
	//remove private pipe
	printf("[CLIENT] removing private pipe\n");
	remove(name);
	
	//Ensure that ACK was received
	if (!strcmp(data, ACK)) {
		printf("[CLIENT] ACK accepted\n");
	}
	else { 
		printf("[CLIENT] did not receive ACK\n");
		exit(1);
	}
	
	//Return ACK to server
	printf("[CLIENT] returning acknowledgement to server\n");
	write(*to_server, data, strlen(data));
	
	
	return priv;
}

void server_fin(int from_client, int to_client) {
	
}

void client_fin(int from_server, int to_server) {
	//write(to_server, FIN, strlen(FIN));
	
	exit(1);
}



