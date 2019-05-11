#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

/* Things to Research */
// 1.) connect() man page
// 2.) recv() man page

int main(int argc, char * argv[]) {

	char *address;
	address = argv[1];

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Connect to an Address
	struct sockaddr_in remote_address;
	remote_address.sin_family = AF_INET;
	remote_address.sin_port = htons(80);	

	// Convert string address to format for address structure
	inet_aton(address, &remote_address.sin_addr.s_addr);

	// Connect socket to remote address
	connect(client_socket, (struct sockaddr *) &remote_address, sizeof(remote_address));

	char request[] = "GET / HTTP/1.1\r\n\r\n";
	char response[4096];

	// Send a HTTP reqest
	send(client_socket, request, sizeof(request), 0);

	// Receive flags parameter set to 0 alike send()
	recv(client_socket, &response, sizeof(response), 0);

	printf("Response from server: %s\n\n", response);

	close(client_socket);

	return 0;

}
