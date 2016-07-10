#pragma once
#include <iostream>
#include <string>
#include "Room.h"
#include "Question.h"
#include <map>
#include <queue>

class DataBase;
class User;
class Game
{
public:
	Game(const std::vector<User*>& players, int questionsNo, DataBase& db);
	~Game();
	void handleFinishGame();
	void sendFirstQuestion();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* currUser);
	int getID();
	bool insertGameDB();
	void initQuestionsFromDB();
	void sendQuestionToAllUsersInGame();
	bool insertGame_db();
	void initQuestionsFrom_db();
	std::map<std::string, int> getResults();
private:
	std::map<std::string, int> _results;
	int _currentTurnAnswers;
	std::vector<Question*> _questions;
	std::vector<User*> _users;
	int _questions_no;
	DataBase& _db;
	int _currQuestionIndex;
	int _id;
	
};