
#include "DataBase.h"
#include <iostream>
#include <string>
#include "User.h"
#include "Room.h"
#include "Question.h"
#include "Validator.h"
#include "sqlite3.h"
#include <map>
#include <vector>
#include <queue>
#include "triviaServer.h"


void main()
{
	PCSTR DEFAULT_PORT = "27015";
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	int iResult = 0;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{

		std::cout << "Socket failed with error " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();

	}
	

	try
	{
		triviaServer ts;
		ts.serve(result);

	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		WSACleanup();
		system("PAUSE");
	}

	
	//do something
}