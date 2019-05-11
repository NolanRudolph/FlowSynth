#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

/* Things to Research */
// 4.) Investigate https://youtu.be/LtXEMwSG5-8?t=2024
// 5.) *** send() man page -- very important

int main() {

	// Creating Server Socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Defining Server Address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// Binding our Socket to our Specefied IP (0.0.0.0 aka INADDR_ANY) and Port
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	// Listen() Function Begins Listening for Connections
	listen(server_socket, 5);

	// Ability to Accept Connections
	int client_socket;
	client_socket = accept(server_socket, NULL, NULL);
	
	char server_message[256] = "Its-a me, a-mario!";

	// Send a message with send()
	send(client_socket, server_message, sizeof(server_message), 0);

	close(server_socket);

	return 0;
}
