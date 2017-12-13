#include "pipe_networking.h"


int main() {
	int to_server;
	int from_server;
	
	from_server = client_handshake( &to_server );
	
	char data[BUFFER_SIZE];
	
	fgets(data, sizeof(data), stdin);
	
	do {
		printf("Sending message to server\n");
		write(to_server, data, sizeof(data));
		
		printf("Reading data from server\n");
		read(from_server, data, strlen(data));
		printf("Data read:%s\n", data);
	}
	while ( strlen(data) > 0 );
	
	
}
