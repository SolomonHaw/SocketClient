#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<winsock2.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in client;
	const char* message;
	char sv_reply[2000];
	int recv_size;

	//1. Initialize Winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	std::cout << "Winsock init complete." << std::endl;

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

	//5. Send data
	message = "GET / HTTP/1.1\r\n\r\n";
	if (send(sock, message, strlen(message), 0) < 0)
	{
		std::cout << "Send failed." << std::endl;
	}
	std::cout << "Data sent." << std::endl;

	//6. Recieve reply from server
	if ((recv_size = recv(sock, sv_reply, 2000, 0)) == SOCKET_ERROR)
	{
		std::cout << "Failed to recieve." << std::endl;
	}
	puts("Reply recieved.");
	return 0;
}