#include "pipe_networking.h"

char* rot13(char *text) {
	char *new_text = (char*)malloc(strlen(text)*sizeof(char));
	
	int x;
	for (x = 0; x < strlen(text); x++) {
		new_text[x] = (text[x] + 13);
	}
	return 0;
}

int main() {
	int to_client;
	int from_client;
	
	from_client = server_handshake( &to_client );
	
	char data[BUFFER_SIZE];
	
	do {
		printf("Waiting for client message\n");
		read(from_client, data, sizeof(data));
		
		if (!strcmp(data, FIN)) {
			printf("[SERVER] received FIN from client\n");
			server_fin(from_client, to_client);
		}
		
		printf("Read data from client:%s\n", data);
		write(to_client, data, strlen(data));
	}
	while ( strlen(data) > 0 );
	
}
