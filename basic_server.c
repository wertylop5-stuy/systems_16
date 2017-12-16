#include <ctype.h>
#include "pipe_networking.h"

void sighandler(int signo) {
	switch(signo) {
		case SIGINT:
			printf("[SERVER] exiting\n");
			
			remove(WKP);
			exit(0);
		break;
	}
}

char* rot13(char *text) {
	char *new_text = (char*)malloc(strlen(text)*sizeof(char));
	
	unsigned char temp;
	int x;
	for (x = 0; x < strlen(text)-1; x++) {
		temp = isalpha(text[x]) ? text[x] + 13 : text[x];
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
	
	signal(SIGINT, sighandler);
	
	while(1) {
	
		from_client = server_handshake( &to_client );
		
		char data[BUFFER_SIZE];
		int len;
		//do {
		printf("[SERVER] press ^C to exit\n");
		while(read(from_client, data, sizeof(data))) {
			//data[0] = 0;
			printf("\n[SERVER] Waiting for client message\n");
			//read(from_client, data, sizeof(data));
			
			if (!strcmp(data, FIN)) {
				printf("[SERVER] received FIN from client\n");
				server_fin(from_client, to_client);
				break;
			}
			
			printf("Read message from client:\n%s", data);
			len = strlen(rot13(data));
			printf("Sending response to client\n");
			write(to_client, rot13(data), len);
		}
		//while ( strlen(data) > 0 );
	
	}
	
}

