#include <iostream>
#include "User.h"
#include <map>
#include <queue>
#include <vector>
#include "Room.h"
Room::Room(int roomId, User* admin, string roomName, int maxUsers, int questionsNo, int questionTime)
{
	this->_id = roomId;
	
	this->_name = roomName;
	this->_maxUsers = maxUsers;
	this->_questionNo = questionsNo;
	this->_questionTime = questionTime;
	
	joinRoom(admin);



}
bool Room::joinRoom(User* user)
{
	int size = _users.size();
	if (size != _maxUsers)
	{
		if (this != NULL)
		{

			_users.push_back(user);
			std::string questionsDetailes = std::to_string(_questionNo) + std::to_string(_questionTime);
			//[1100 questionsNumber questionTimeInSec]
			user->send("110" + questionsDetailes);
			this->sendMessage("108" + getUsersListMessage());
			return true;
		}
		user->send("1102");  //room does not exist
		return false;
	}
	else
	{
		user->send("1101"); //room is full
	}
	return false;
}
void Room::leaveRoom(User* user)
{
	if (user->getRoom() == this) // might be problematic
	{
		int spot = 0;
		bool found = false;
		int size = _users.size();
		for (int i = 0; i < size; i++)
		{
			if ((_users[i] == user) && (found == false))
			{
				_users.erase(_users.begin() + spot);
				found = true;
			}
			spot++;
		}
		if (found)
		{
			user->send("1120"); // success
			this->sendMessage(user, this->getUsersListMessage());
		}
	}
}
User* Room::getAdmin()
{
	return _admin;
}
int Room::closeRoom(User* user)
{
	
	if (user == this->_admin)
	{
		int id = this->_id;

		this->sendMessage("116");

		for (User* i : _users)
		{
			if (i != this->_admin)
			{
				i->clearRoom();
			}
		}
		return id;

	}
	return -1;
}
std::vector<User*> Room::getUsers()
{
	return _users;
}
std::string Room::getUsersListMessage()  //every byte is seperated by space
{
	string output = "108";
	output += _users.size();
	for (User* i : _users)
	{
		string tempUserName = i->getUsername();
		output += tempUserName.length();
		output += tempUserName;
	}
	return output;
}
int Room::getQuestionsNo()
{
	return this->_questionNo;
}
int Room::getId()
{
	return this->_id;
}
std::string Room::getName()
{
	return this->_name;
}
std::string Room::getUsersAsString(std::vector<User*> usersList, User* excludeUser)    //the names will be seperated by ", "
{
	string excludedName = excludeUser->getUsername();
	string tempUserName = "";
	string output = "";
	int size = _users.size();
	for (int i = 0; i < size; i++)
	{
		tempUserName = _users[i]->getUsername();
		if (tempUserName != excludedName)
		{
			output += tempUserName;
			if (i != _users.size() - 2)
			{
				output += ", ";
			}
		}
	}
	return output;
}
void Room::sendMessage(std::string message)
{
	int size = _users.size();
	for (int i = 0; i < size; i++)
	{
		try
		{
			_users[i]->send(message);
		}
		catch (std::exception &ex)
		{
			std::cout << ex.what() << std::endl;
			std::cout << "Error in send Message - " << message;
		}
	}
}
void Room::sendMessage(User* excludeUser, std::string message)
{
	int size = _users.size();
	for (int i = 0; i <size; i++)
	{
		if (_users[i] != excludeUser)
		{
			try
			{
				_users[i]->send(message);
			}
			catch (std::exception &ex)
			{
				std::cout << ex.what() << std::endl;
				std::cout << "Error in send Message - " << message;
			}
		}
	}
}
int Room::getNumberOfUsers(){
	return _users.size();
}