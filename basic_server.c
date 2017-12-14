#include "pipe_networking.h"

char* rot13(char *text) {
	char *new_text = (char*)malloc(strlen(text)*sizeof(char));
	
	unsigned char temp;
	int x;
	for (x = 0; x < strlen(text)-1; x++) {
		temp = text[x] + 13;
		if ((text[x] >= 'A' && text[x] <= 'Z') && temp > 'Z') {
			temp -= 26;
		}
		else if ((text[x] >= 'a' && text[x] <= 'z') && temp > 'z') {
			temp -= 26;
		}
		new_text[x] = temp;
	}
	
	return new_text;
}

int main() {
	int to_client;
	int from_client;
	
	from_client = server_handshake( &to_client );
	
	char data[BUFFER_SIZE];
	int len;
	do {
		printf("\nWaiting for client message\n");
		read(from_client, data, sizeof(data));
		
		if (!strcmp(data, FIN)) {
			printf("[SERVER] received FIN from client\n");
			server_fin(from_client, to_client);
		}
		
		printf("Read message from client:\n%s", data);
		len = strlen(rot13(data));
		printf("Sending response to client\n");
		write(to_client, rot13(data), len);
	}
	while ( strlen(data) > 0 );
	
}
