#include <iostream>
#include "Game.h"
#include <string>
#include "Helper.h"
#include "Room.h"
#include "User.h"
User::User(string username, SOCKET  sock)
{
	this->_username = username;
	this->_sock = sock;
	_currRoom = NULL;


}

void User::setUserName(string newUsername)
{
	this->_username = newUsername;
}

void User::send(string message)
{

	Helper::sendData(this->_sock, message);

}
SOCKET User::getSocket()
{
	return this->_sock;

}
Room* User::getRoom()
{
	return this->_currRoom;

}
Game* User::getGame()
{
	return _currGame;

}
string User::getUsername()
{
	return _username;

}
void User::setGame(Game* gm)
{

	this->_currGame = gm;
	this->_currRoom = NULL;

}
bool User::createRoom(int roomId, string roomName, int maxUsers, int questionsNo, int questionTime)
{
	if (this->_currRoom == NULL)
	{

		this->_currRoom = new Room(roomId, this, roomName, maxUsers, questionsNo, questionTime);

		this->send("1140");
		return true;
	}

	this->send("1141");
	return false;
}
bool User::joinRoom(Room* newRoom)
{
	if ((this->_currRoom == NULL) && (newRoom != NULL))
	{
		
			this->_currRoom = newRoom;
		
		return true;
	}

	return false;
}
void User::leaveRoom()
{

	if (this->_currRoom != NULL)
	{

		this->_currRoom = NULL;

	}

}
int User::closeRoom()
{
	int roomNum = -1;

	if (this->_currRoom != NULL)
	{
		
			roomNum = this->_currRoom->getId();
			free(this->_currRoom);
			this->_currRoom = NULL;
		

	}
	return roomNum;
}
void User::clearRoom()
{
	this->_currRoom = NULL;

}
bool User::leaveGamePrimal()
{
	
	if (this->_currGame != NULL)
	{
		
		this->_currGame = NULL;
		return true;
	}

	return false;
}
void User::clearGame()
{

	this->_currGame = NULL;
	

}
