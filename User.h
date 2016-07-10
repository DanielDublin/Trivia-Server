#pragma once
#include <iostream>
#include <string>
#include <map>
#include <queue>
#include "Helper.h"


class Game;
class Room;
class User
{
public:
	User(string username, SOCKET  sock);
	void send(string message);
	void setGame(Game* gm);
	SOCKET getSocket();
	Room* getRoom();
	Game* getGame();
	string getUsername();
	void clearGame();
	void clearRoom();
	bool createRoom(int roomId, string roomName, int maxUsers, int questionsNo, int questionTime);
	bool joinRoom(Room* newRoom);
	void leaveRoom();
	int closeRoom();
	bool leaveGamePrimal();
	void setUserName(std::string newUsername);
private:
	string _username;
	SOCKET  _sock;
	Room* _currRoom;
	Game* _currGame;

};