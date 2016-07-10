#include <iostream>
#include <string>
#include "Room.h"
#include "User.h"
#include "RecievedMessage.h"
#include <map>
#include <queue>
RecievedMessage::RecievedMessage(SOCKET sock, int messageCode)
{
	_messeageCode = messageCode;
	_sock = sock;
	_user = NULL;
}
RecievedMessage::RecievedMessage(SOCKET sock, int messageCode, vector<string> values)
{
	_messeageCode = messageCode;
	_sock = sock;
	_user = NULL;
	_values = values;
}
SOCKET RecievedMessage::getSocket()
{
	return _sock;
}
User* RecievedMessage::getUser()
{
	return _user;
}
void RecievedMessage::setUser(User* user)
{
	_user = user;
}
int RecievedMessage::getMessageCode()
{
	return _messeageCode;
}
vector<string>& RecievedMessage::getValues()
{
	return _values;
}

