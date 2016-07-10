#pragma once
#include <iostream>
#include <string>
#include "Room.h"
#include "User.h"
#include <map>
#include <queue>
using std::vector;


class User;
class RecievedMessage
{
public:
	RecievedMessage(SOCKET sock, int messageCode);
	RecievedMessage(SOCKET sock, int messageCode, vector<string> values);
	SOCKET getSocket();
	User* getUser();
	void setUser(User* user);
	int getMessageCode();
	vector<string>& getValues();
private:
	SOCKET _sock;
	User* _user;
	int _messeageCode;
	vector<string> _values;
};