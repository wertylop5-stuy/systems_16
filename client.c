#include<signal.h>

#include "pipe_networking.h"

int to_server;
int from_server;


void sighandler(int signo) {
	switch(signo) {
		case SIGINT:
			printf("[CLIENT] exiting, sending termination\n");
			
			client_fin(from_server, to_server);
		break;
	}
}

int main() {
	from_server = client_handshake( &to_server );
	
	char data[BUFFER_SIZE];
	
	//signal(SIGINT, sighandler);
	printf("[CLIENT] Press ^C to exit\n");
	
	//fgets(data, sizeof(data), stdin);
	
	do {
		fgets(data, sizeof(data), stdin);
		printf("Sending message to server\n");
		write(to_server, data, sizeof(data));
		
		printf("Reading data from server\n");
		read(from_server, data, strlen(data));
		printf("Data read:%s\n", data);
	}
	while ( strlen(data) > 0 );
	
	
}
