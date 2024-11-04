#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<winsock2.h>
#include <ws2tcpip.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include "SocketClient.h"

WSADATA wsa;
SOCKET sock;
struct sockaddr_in client;
const char* message;
char sv_reply[256];
int nb_sock;

WSAPOLLFD fds[1];

int main(int argc, char* argv[])
{
	//1. Initialize Winsock
	printf("\nInitialising client...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	std::cout << "Client init complete." << std::endl;

	//2. Create the socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket. Error: %d", WSAGetLastError());
		ExitProcess(EXIT_FAILURE);
	}
	std::cout << "Socket created." << std::endl;

	//3. Define the socket structure
	ZeroMemory(&client, sizeof(client));
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	client.sin_family = AF_INET;
	client.sin_port = htons(80);

	//4. Connect to local server
	if (connect(sock, (struct sockaddr*)&client, sizeof(client)) < 0) 
	{
		std::cout << "Connection error." << std::endl;
		ExitProcess(EXIT_FAILURE);
	}
	puts("Connected");
	
	//Blocking code
	////5. Send data
	//message = "GET / HTTP/1.1\r\n\r\n";
	//if (send(sock, message, strlen(message), 0) < 0)
	//{
	//	std::cout << "Send failed." << std::endl;
	//}
	//std::cout << "Data sent." << std::endl;

	////6. Recieve reply from server
	//if (recv(sock, sv_reply, 2000, 0) == SOCKET_ERROR)
	//{
	//	printf("Failed to recieve. Error: %d\n", WSAGetLastError());
	//}
	//else {
	//	puts("Reply recieved.");
	//}
	turn_socket_into_nonblocking();

	while (1) {
		//4c. initialize struct and start polling 
		fds[0].fd = sock;
		fds[0].events = POLLIN | POLLOUT;
		
		int results = WSAPoll(fds, 1, -1);
			if (results < 1) {
			printf("Poll error\n");
			break;
			}

		//5. Check if socket is ready for sending
		if (fds[0].revents & POLLOUT) {
			message = "GET / HTTP/1.1\r\n\r\n";
			int sent = send(sock, message, strlen(message), 0);
			if (sent < 0) {
				if (errno == EAGAIN) {
					//Socket is not ready to send, try again later
					printf("Send would block, retrying...\n");
				}
				else
					printf("Send error\n");
					break;
			}
			else {
				printf("Sent: %s\n", message);
			}
			fds[0].events = POLLIN; //tell socket to change to write
		}

		//6. Check if socket is ready for recieving
		if (fds[0].revents & POLLIN) {
			int rec = recv(sock, sv_reply, sizeof(sv_reply) - 1, 0);
			if (rec < 0) {
				if (errno == EAGAIN) {
					// The socket is not ready to receive, retry later
					printf("Receive would block, retrying...\n");
				}
				else {
					printf("recieve error\n");
					break;
				}
			}
			else if (rec == 0) {
				// Connection closed
				printf("Connection closed by server.\n");
				break;
			}
			else {
				sv_reply[rec] = '\0'; // Null-terminate the string
				printf("Received: %s\n", sv_reply);
				break;
			}
		}
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

int turn_socket_into_nonblocking() {
	
	//4b.set socket to non-blocking (client socket must match server's)
	u_long ul = 1;
	nb_sock = ioctlsocket(sock, FIONBIO, &ul);
	if (nb_sock == SOCKET_ERROR) {
		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("ioctlsocket() is OK!\n");
	return 0;
}