#include "triviaServer.h"

triviaServer::triviaServer() :_db()
{
	_roomIdSequence = 0;
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
		throw std::exception("socket failed");
}
triviaServer::~triviaServer()
{
	for (std::map<int, Room*>::iterator it = _roomsList.begin(); it != _roomsList.end(); ++it)
		delete it->second;
	_roomsList.clear();
	for (std::map<SOCKET, User*>::iterator it = _connectedUsers.begin(); it != _connectedUsers.end(); ++it)
		delete it->second;
	_connectedUsers.clear();
	if (closesocket(_socket) == SOCKET_ERROR)
		throw std::exception("close socket failed");
}
void triviaServer::serve(struct addrinfo* a)
{
	bindAndListen(a);
	std::thread(&triviaServer::handleRecievedMessages, this).detach();
	while (true)
	{
		try
		{
			_accept(listenSocket);
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}
void triviaServer::bindAndListen(struct addrinfo* a)
{
	if (bind(_socket, a->ai_addr, (int)a->ai_addrlen) == SOCKET_ERROR)
		throw std::exception("socket binding failed");
	freeaddrinfo(a);
	if (listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception("socket listening failed");
}
void triviaServer::_accept(SOCKET listenSocket)
{
	SOCKADDR_IN sockServ;
	int sockServLen = sizeof(sockServ);
	SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&sockServ, &sockServLen);
	if (clientSocket == INVALID_SOCKET)
	{
		if (closesocket(_socket) == SOCKET_ERROR)
			throw std::exception("close socket failed");
		throw std::exception("socket accept failed");
	}
	User* newUser = new User("", clientSocket);
	_connectedUsers.insert(std::pair<SOCKET, User*>(clientSocket, newUser));
	std::thread(&triviaServer::clientHandler, this, std::ref(clientSocket)).detach();
}
void triviaServer::clientHandler(SOCKET client_socket)
{
	RecievedMessage* msg;
	int code = Helper::getMessageTypeCode(client_socket);
	while (code != 0 && code != 201)
	{
		try
		{
			msg = buildRecieveMessage(client_socket, code);
			addRecievedMessage(msg);
			code = Helper::getMessageTypeCode(client_socket);
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
			msg = buildRecieveMessage(client_socket, 201);
			addRecievedMessage(msg);
			break;
		}
	}
	msg = buildRecieveMessage(client_socket, 201);
	addRecievedMessage(msg);
}
Room* triviaServer::getRoomById(int roomId)
{
	for (std::map<int, Room*>::const_iterator it=_roomsList.begin(); it != _roomsList.end(); ++it)
	{
		if (it->second->getId()==roomId)
		{
			return it->second;
		}
	}
	return NULL;
}
User* triviaServer::getUserByName(std::string username)
{
	for (std::map<SOCKET, User*>::const_iterator it = _connectedUsers.begin(); it != _connectedUsers.end(); ++it)
	{
		if ((it->second->getUsername()).compare(username)==0)
		{
			return it->second;
		}
	}
	return NULL;
}
User* triviaServer::getUserBySocket(SOCKET client_socket)
{
	for (std::map<SOCKET, User*>::const_iterator it = _connectedUsers.begin(); it != _connectedUsers.end(); ++it)
	{
		if (it->first==client_socket)
		{
			return it->second;
		}
	}
	return NULL;
}
void triviaServer::handleRecievedMessages()
{
	RecievedMessage r(0,0);
	while (true)
	{
		r = *(_queRcvMessages.dequeue());
		r.setUser(getUserBySocket(r.getSocket()));
		if (r.getMessageCode() == 200)
		{
			handleSignin(&r);
		}
		else if (r.getMessageCode() == 201)
		{
			handleSignout(&r);
		}
		else if (r.getMessageCode() == 203)
		{
			handleSignup(&r);
		}
		else if (r.getMessageCode() == 222)
		{
			handleLeaveGame(&r);
		}
		else if (r.getMessageCode() == 217)
		{
			handleStartGame(&r);
		}
		else if (r.getMessageCode() == 219)
		{
			handlePlayerAnswer(&r);
		}
		else if (r.getMessageCode() == 213)
		{
			handleCreateRoom(&r);
		}
		else if (r.getMessageCode() == 215)
		{
			handleCloseRoom(&r);
		}
		else if (r.getMessageCode() == 209)
		{
			handleJoinRoom(&r);
		}
		else if (r.getMessageCode() == 211)
		{
			handleLeaveRoom(&r);
		}
		else if (r.getMessageCode() == 207)
		{
			handleGetUsersInRoom(&r);
		}
		else if (r.getMessageCode() == 205)
		{
			handleGetRooms(&r);
		}
		else if (r.getMessageCode() == 223)
		{
			handleGetBestScores(&r);
		}
		else if (r.getMessageCode() == 225)
		{
			handleGetPersonalStatus(&r);
		}
		else
		{
			safeDeleteUesr(&r);
		}
	}

}
void triviaServer::safeDeleteUesr(RecievedMessage* msg)
{
	try
	{
		SOCKET soc = msg->getSocket();
		handleSignout(msg);
		_connectedUsers.erase(_connectedUsers.find(soc));
		closesocket(soc);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
User* triviaServer::handleSignin(RecievedMessage* msg) //200
{
	int i = 0;
	if (_db.isUserAndPassMatch(msg->getValues()[0], msg->getValues()[1]))
	{
		if (getUserByName(msg->getValues()[0]))
		{
			Helper::sendData(msg->getSocket(), "1022");
			return NULL;
		}
		else
		{
			User* newuser=getUserByName(msg->getValues()[0]);
			newuser->setUserName(msg->getValues()[0]);
			_connectedUsers[msg->getSocket()] = newuser;
			Helper::sendData(msg->getSocket(), "1020");
			return newuser;
		}
	}
	else
	{
		Helper::sendData(msg->getSocket(), "1021");
		return NULL;
	}
}
void triviaServer::handleSignout(RecievedMessage* msg) //201
{

	if (_connectedUsers.find(msg->getSocket()) != _connectedUsers.end())
	{
		if (handleCloseRoom(msg))
			if (handleLeaveRoom(msg))
				handleLeaveGame(msg);
	}
}
bool triviaServer::handleSignup(RecievedMessage* msg) //203
{
	Validator v(msg->getValues()[0], msg->getValues()[1]);
	if (v.isPasswordValid())
	{
		if (v.isUsernameValid())
		{
			if (_db.isUserExists(msg->getValues()[0]))
			{
				Helper::sendData(msg->getSocket(), "1042");
				return false;
			}
			else
			{
				if (_db.addNewUserInDataBase(msg->getValues()[0], msg->getValues()[1], msg->getValues()[2]))
				{
					Helper::sendData(msg->getSocket(), "1040");
					return true;
				}
				else
				{
					Helper::sendData(msg->getSocket(), "1043");
					return false;
				}
			}
		}
		else
		{
			Helper::sendData(msg->getSocket(), "1043");
			return false;
		}
	}
	else
	{
		Helper::sendData(msg->getSocket(), "1041");
		return false;
	}
	Helper::sendData(msg->getSocket(), "1044");
	return false;
}
void triviaServer::handleLeaveGame(RecievedMessage* msg) //222
{
	getUserBySocket(msg->getSocket())->leaveGamePrimal();
}
void triviaServer::handleStartGame(RecievedMessage* msg) //217
{
	try
	{
		Game g(msg->getUser()->getRoom()->getUsers(), msg->getUser()->getRoom()->getQuestionsNo(), _db);
		int help = (msg->getUser()->getRoom()->getId());
		std::map<int, Room*>::iterator it = _roomsList.find(help);
		_roomsList.erase(it);
		_connectedUsers.at(msg->getSocket()) = msg->getUser();
		g.sendFirstQuestion();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		Helper::sendData(msg->getUser()->getRoom()->getAdmin()->getSocket(), "1180");
	}
}
void triviaServer::handlePlayerAnswer(RecievedMessage* msg) //219
{
	if (msg->getUser()->getGame())
	{
		if (!msg->getUser()->getGame()->handleAnswerFromUser(msg->getUser(), std::stoi(msg->getValues()[0]), std::stoi(msg->getValues()[1])))
		{
			Game* g = msg->getUser()->getGame();
			delete g;
		}
	}
}
bool triviaServer::handleCreateRoom(RecievedMessage* msg) //213
{
	if (msg->getUser())
	{
		_roomIdSequence++;
		if (msg->getUser()->createRoom(_roomIdSequence, msg->getValues()[0], stoi(msg->getValues()[1]), stoi(msg->getValues()[2]), stoi(msg->getValues()[3])))
		{
			_roomsList[_roomIdSequence] =msg->getUser()->getRoom();
			_connectedUsers.at(msg->getSocket()) = msg->getUser();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
bool triviaServer::handleCloseRoom(RecievedMessage* msg) //215
{
	if (msg->getUser())
	{
		if (msg->getUser()->getRoom())
		{
			if (msg->getUser()->closeRoom() != (-1))
			{
				std::map<int, Room*>::iterator it;
				int help = (msg->getUser()->getRoom()->getId());
				it = _roomsList.find(help);
				_roomsList.erase(it);
				return true;
			}
		}
	}
	return false;
}
bool triviaServer::handleJoinRoom(RecievedMessage* msg) //209
{
	if (msg->getUser())
	{
		int id = std::stoi(msg->getValues()[0]);
		if (getRoomById(id))
		{
			if (msg->getUser()->joinRoom(getRoomById(id)))
			{
				Helper::sendData(msg->getSocket(), "1100");
				_connectedUsers.at(msg->getSocket()) = msg->getUser();
				return true;
			}
			Helper::sendData(msg->getSocket(), "1101");
		}
		Helper::sendData(msg->getSocket(), "1102");
	}
	return false;
}
bool triviaServer::handleLeaveRoom(RecievedMessage* msg) //211
{
	if (msg->getUser())
	{
		if (msg->getUser()->getRoom())
		{
			msg->getUser()->leaveRoom();
			return true;
		}
	}
	return false;
}
void triviaServer::handleGetUsersInRoom(RecievedMessage* msg) //207
{
	if (msg->getUser())
	{
		if (std::stoi(msg->getValues()[0]) != (-1))
		{
			if (getRoomById(std::stoi(msg->getValues()[0])))
			{
				Helper::sendData(msg->getSocket(), msg->getUser()->getRoom()->getUsersListMessage());
			}
		}
		else
		{
			Helper::sendData(msg->getSocket(), "1080");
		}
	}
}
void triviaServer::handleGetRooms(RecievedMessage* msg) //205
{
	std::string msgToSend = "106";
	msgToSend += Helper::getPaddedNumber(_roomsList.size(), 4);
	for (std::map<int, Room*>::iterator it = _roomsList.begin(); it != _roomsList.end(); ++it)
	{
		msgToSend += Helper::getPaddedNumber(it->first, 4);
		msgToSend += Helper::getPaddedNumber(it->second->getName().size(), 2);
		msgToSend += it->second->getName();
	}
	Helper::sendData(msg->getSocket(), msgToSend);
}
void triviaServer::handleGetBestScores(RecievedMessage* msg) //223
{
	std:vector<std::string> m = _db.getBestScores();
	std::string s = "124";
	std::string username= "", score= "";
	int size = m.size();;
	for (int i = 0; i < size; i++)
	{
		std::string temp = m[i];
		char c = '4';
		int spot = 0;
		while (temp[spot] != ' ')
		{
			c = temp[spot];
			username += c;
			spot++;

		}
		spot++;
		while (spot != temp.size())
		{
			c = temp[spot];
			score += c;
			spot++;

		}
		

		s = s + Helper::getPaddedNumber(username.size(), 2) + username + Helper::getPaddedNumber(std::stoi(score), 6);
		username = "";
		score = "";
	}
	size = m.size();
	for (int i = 0; i < 3 - size; i++)
		s += "00";
	Helper::sendData(msg->getSocket(), s);
}
void triviaServer::handleGetPersonalStatus(RecievedMessage* msg) //225
{
	std::vector<std::string> v = _db.getPersonalStatus(msg->getUser()->getUsername());
	std::string msgToSend = "126" + v[0] + v[1] + v[2] + v[3];
	Helper::sendData(msg->getSocket(), msgToSend);
}
void triviaServer::addRecievedMessage(RecievedMessage* msg) //225
{
	_queRcvMessages.enqueue(msg);
}
RecievedMessage* triviaServer::buildRecieveMessage(SOCKET client_socket, int msgCode) //225
{
	int l;
	std::vector<std::string> v;
	int msg=Helper::getMessageTypeCode(client_socket);
	switch (msg)
	{
		case 200:
		{
			l = Helper::getIntPartFromSocket(client_socket, 2);
			v[0] = Helper::getStringPartFromSocket(client_socket, l);
			l = Helper::getIntPartFromSocket(client_socket, 2);
			v[1] = Helper::getStringPartFromSocket(client_socket, l);
			RecievedMessage r(client_socket, msg, v);
			return &r;
		}
		case 201:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 203:
		{
			l = Helper::getIntPartFromSocket(client_socket, 2);
			v[0] = Helper::getStringPartFromSocket(client_socket, l);
			l = Helper::getIntPartFromSocket(client_socket, 2);
			v[1] = Helper::getStringPartFromSocket(client_socket, l);
			l = Helper::getIntPartFromSocket(client_socket, 2);
			v[2] = Helper::getStringPartFromSocket(client_socket, l);
			RecievedMessage r(client_socket, msg, v);
			return &r;
		}
		case 205:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 207:
		{
			v[0] = Helper::getStringPartFromSocket(client_socket, 4);
			RecievedMessage r(client_socket, msg, v);
			return &r;
		}
		case 209:
		{
			v[0] = Helper::getStringPartFromSocket(client_socket, 4);
			RecievedMessage r(client_socket, msg, v);
			return &r;
		}
		case 211:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 213:
		{
			l = Helper::getIntPartFromSocket(client_socket, 2);
			v[0] = Helper::getStringPartFromSocket(client_socket, l);
			v[1] = Helper::getStringPartFromSocket(client_socket, 1);
			v[2] = Helper::getStringPartFromSocket(client_socket, 2);
			v[3] = Helper::getStringPartFromSocket(client_socket, 2);
			RecievedMessage r(client_socket, msg, v);
			return &r;
		}
		case 215:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 217:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 219:
		{
			v[0] = Helper::getStringPartFromSocket(client_socket, 1);
			v[1] = Helper::getStringPartFromSocket(client_socket, 2);
			RecievedMessage r(client_socket, msg, v);
			return &r;
		}
		case 222:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 223:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 225:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		case 299:
		{
			RecievedMessage r(client_socket, msg);
			return &r;
		}
		default:
		{
			return nullptr;
		}
	}
}