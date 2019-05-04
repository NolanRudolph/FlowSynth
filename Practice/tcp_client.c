#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

/* Thing to Research */
// 1.) ***Socket() man page
// 2.) Address families like AF_INET
// 3.) sin_addr structure attributes
// 4.) connect() man page

int main(int argc, char* agrv[]) {
	// Creating a Socket
	int network_socket;
	// socket(domain of socket (generally AF_INET, a constant), TCP socket, protocol)
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Specifying an Address for the Socket (Address family is same as parameter one of socket())
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;

	// Specify the port, 9002 is random high port to avoid conflicts
	server_address.sin_port = htons(9002);

	// server_address holds information about the address
	// sin_addr is a structure itself that holds lots of data
	// s_addr is the real server address
	// INADDR_ANY is synonymous with 0.0.0.0
	server_address.sin_addr.s_addr = INADDR_ANY;

	// Returns an integer for error handling
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	// Error Handling
	if (connection_status == -1) {
		printf("There was an error making a connection to the remote socket.\n\n");
	}

	// If we send data, we receive a response from the server in recv
	char server_response[256];
	recv(network_socket, &server_response, sizeof(server_response), 0);
	
	// Printing the Server's Response
	printf("The server responded with: %s\n", server_response);

	// Avoiding data leaks
	close(network_socket);
	
	return 0;
}
