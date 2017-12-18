#include "pipe_networking.h"

static void validate(char *entity, char *data, const char* const expected) {
	if (!strcmp(data, expected)) {
		printf("[%s] received expected value\n", entity);
	}
	else {
		fprintf(stderr, "[%s] did not receive expected value\n", entity);
		exit(1);
	}
}

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
	strncpy(data, "", sizeof(data));
	read(wkp, data, sizeof(data));
	
	//Check if ACK and data match
	validate("SERVER", data, ACK);
	printf("[SERVER] connection established\n");
	
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
	
	//attempt to open wkp
	printf("[CLIENT] opening well known pipe\n");
	*to_server = open(WKP, O_WRONLY);
	
	//The private pipe the server will connect to
	printf("[CLIENT] creating priv pipe\n");
	char name[HANDSHAKE_BUFFER_SIZE];
	sprintf(name, "%d", getpid());
	int priv = mkfifo(name, 0644);
	
	//send private pipe name to server
	printf("[CLIENT] sending priv name to server\n");
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
	validate("CLIENT", data, ACK);
	
	//Return ACK to server
	printf("[CLIENT] returning acknowledgement to server\n");
	write(*to_server, data, strlen(data));
	
	
	return priv;
}

void server_fin(int from_client, int to_client) {
	char data[HANDSHAKE_BUFFER_SIZE];
	//Server sends ACK to client
	printf("[SERVER] sending ACK to client\n");
	write(to_client, ACK, strlen(ACK));
	
	//writing twice in a row is some sort of race condition
	//Sometimes the messages are received seperately, but
	//sometimes they may be read at the same time by the client
	/*
	//Server sends FIN to client
	printf("[SERVER] sending FIN to client\n");
	write(to_client, FIN, strlen(FIN));
	*/
	
	//Server now waits for client ACK
	printf("[SERVER] waiting for client ACK\n");
	read(from_client, data, sizeof(data));
	validate("SERVER", data, ACK);
}

void client_fin(int from_server, int to_server) {
	char data[HANDSHAKE_BUFFER_SIZE];
	//Client sends FIN to server
	printf("[CLIENT] sending FIN to server\n");
	write(to_server, FIN, strlen(FIN));
	
	//Client waits to receive ACK
	printf("[CLIENT] waiting for server ACK\n");
	read(from_server, data, sizeof(data));
	validate("CLIENT", data, ACK);
	
	/*
	//Client now waits for server FIN
	printf("[CLIENT] waiting for server FIN\n");
	read(from_server, data, sizeof(data));
	printf("[CLIENT] got %s\n", data);
	validate("CLIENT", data, FIN);
	*/
	
	//Client returns ACK and closes
	printf("[CLIENT] sending ACK to server\n");
	write(to_server, ACK, strlen(ACK));
	
	exit(0);
}



