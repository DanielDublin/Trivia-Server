#pragma once
#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <vector>


class User;
class Room
{
public:
	Room(int roomId, User* admin, std::string roomName, int maxUsers, int questionsNo, int questionTime);
	bool joinRoom(User* user);
	void leaveRoom(User* user);
	int closeRoom(User* user);
	std::vector<User*> getUsers();
	std::string getUsersListMessage();
	int getQuestionsNo();
	int getId();
	std::string getName();
	std::string getUsersAsString(std::vector<User*> usersList, User* excludeUser);
	void sendMessage(std::string message);
	void sendMessage(User* excludeUser, std::string message);
	User* getAdmin();
	int getNumberOfUsers();
	
	

private:
	std::vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionNo;
	std::string _name;
	int _id;
};