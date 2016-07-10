#pragma once
#include "RecievedMessage.h"
#include <stdio.h>
#include <Windows.h>
#include "DataBase.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "SafeQueue.h"
#include <string>
#include <thread>
#include "Room.h"
#include "User.h"
#include <map>
#include <queue>
#include<iostream>
#include<exception>
#include<string>
#include "Game.h"
#include "Helper.h"
#include "SafeQueue.h"
#include "Validator.h"
class triviaServer
{
public:
	triviaServer();
	~triviaServer();
	void serve(struct addrinfo* a);
	void bindAndListen(struct addrinfo* a);
	void _accept(SOCKET listenSocket);
	void clientHandler(SOCKET client_socket);
	Room* getRoomById(int roomId);
	User* getUserByName(std::string username);
	User* getUserBySocket(SOCKET client_socket);
	void handleRecievedMessages();
	void safeDeleteUesr(RecievedMessage* msg);
	User* handleSignin(RecievedMessage* msg); //200
	void handleSignout(RecievedMessage* msg); //201
	bool handleSignup(RecievedMessage* msg); //203
	void handleLeaveGame(RecievedMessage* msg); //222
	void handleStartGame(RecievedMessage* msg); //217
	void handlePlayerAnswer(RecievedMessage* msg); //219
	bool handleCreateRoom(RecievedMessage* msg); //213
	bool handleCloseRoom(RecievedMessage* msg); //215
	bool handleJoinRoom(RecievedMessage* msg); //209
	bool handleLeaveRoom(RecievedMessage* msg); //211
	void handleGetUsersInRoom(RecievedMessage* msg); //207
	void handleGetRooms(RecievedMessage* msg); //205
	void handleGetBestScores(RecievedMessage* msg); //223
	void handleGetPersonalStatus(RecievedMessage* msg); //225
	void addRecievedMessage(RecievedMessage* msg); //225
	RecievedMessage* buildRecieveMessage(SOCKET client_socket, int msgCode); //225
private:
	SOCKET _socket;
	std::map<SOCKET, User*> _connectedUsers;
	std::map<int, Room*> _roomsList;
	SafeQueue<RecievedMessage*> _queRcvMessages;
	int _roomIdSequence;
	DataBase _db;
	SOCKET listenSocket;
};