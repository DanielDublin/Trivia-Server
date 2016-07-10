#pragma once

#include <iostream>
#include <string>
#include "User.h"
#include "Room.h"
#include "Question.h"
#include <map>
#include "sqlite3.h"
#include <queue>
#include "Game.h"



class Game;
class DataBase
{
public:
	DataBase();
	DataBase(DataBase *db);
	~DataBase();
	bool isUserExists(string username);
	bool addNewUserInDataBase(string username, string password, string email);
	bool isUserAndPassMatch(string username, string password);
	std::map<int, Game*> getGames();
	std::vector<Question*> initQuestions(int questionsNo);
	std::vector<string> getBestScores();
	std::vector<string> getPersonalStatus(string username);
	int insertNewGame();
	bool updateGameStatus(Game* gm);
	bool updateGameStatus(int status);
	bool addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime);
private:
	int static callbackCount(void*, int, char**, char**);
	int static callbackQuestions(void*, int, char**, char**);
	int static callbackBestScores(void*, int, char**, char**);
	int static callbackPersonalStatus(void*, int, char**, char**);
	std::map <string, std::vector<string>> _users;
	std::vector<Question> _questions;
	std::map<int, Game*> _games;
	sqlite3 *_db;
};