#include <stdio.h>
#include "DataBase.h"
#include <string>
#include "Room.h"
#include "Game.h"
#include <vector>
#include <map>
#include <queue>
Game::Game(const std::vector<User*>& players, int questionsNo, DataBase& _db) :_db(this->_db)
{
	
		
	
		try
		{
			int temp = _db.insertNewGame(); //-1 if game invalid
				if (temp == -1)
				{
					throw temp;

				}

				_db.initQuestions(questionsNo);
				this->_users = players;
				int size = players.size();
				for (int i = 0; i < size; i++)
				{
					std::string name = players[i]->getUsername();
					_results.insert(std::make_pair(name, 0));
					_users[i]->setGame(this);
				}

					
		}
		catch (int a)
		{
			std::cout << "could not insert new game" << std::endl;

		}
		

}
Game::~Game()
{
	int size = _questions.size();
	for (int i = 0; i <size ; i++)
	{
		delete _questions[i];
		_questions.pop_back();
	}
	size = _users.size();
	for (int i = 0; i < size; i++)
	{
		_users.pop_back();
	}
}
void Game::sendQuestionToAllUsersInGame()
{
	std::string msg = "118" + Helper::getPaddedNumber(_questions[0]->getQuestion().size(), 3) + _questions[0]->getQuestion();
	msg += Helper::getPaddedNumber(_questions[_currQuestionIndex]->getAnswers()[0].size(), 3) + _questions[_currQuestionIndex]->getAnswers()[0];
	msg += Helper::getPaddedNumber(_questions[_currQuestionIndex]->getAnswers()[1].size(), 3) + _questions[_currQuestionIndex]->getAnswers()[1];
	msg += Helper::getPaddedNumber(_questions[_currQuestionIndex]->getAnswers()[2].size(), 3) + _questions[_currQuestionIndex]->getAnswers()[2];
	msg += Helper::getPaddedNumber(_questions[_currQuestionIndex]->getAnswers()[3].size(), 3) + _questions[_currQuestionIndex]->getAnswers()[3];
	_currentTurnAnswers = 0;
	for (int i = 0; i < _users.size(); i++)
	{
		try
		{
			_users[i]->send(msg);
		}
		catch (std::exception &ex)
		{
			break;
		}
	}
}
void Game::handleFinishGame()
{
	std::string msg = "121" + Helper::getPaddedNumber(_users.size(), 1);
	for (int i = 0; i < _users.size(); i++)
	{
		msg += Helper::getPaddedNumber(_users[i]->getUsername().size(), 2);
		msg += _users[i]->getUsername();
	}
	_db.updateGameStatus(this);
	for (int i = 0; i < _users.size(); i++)
	{
		try
		{
			_users[i]->send(msg + Helper::getPaddedNumber(_results.at(_users[i]->getUsername()), 2)); 
		}
		catch (std::exception &ex)
		{
			break;
		}
		_users[i]->setGame(nullptr);
	}
}
void Game::sendFirstQuestion()
{
	sendQuestionToAllUsersInGame();
}
bool Game::handleNextTurn()
{
	if (_users.size())
	{
		
			if (_currQuestionIndex == _questions_no)
			{
				handleFinishGame();
				return false;
			}
			else
			{
				_currQuestionIndex++;
				sendQuestionToAllUsersInGame();
			}
		
	}
	else
	{
		handleFinishGame();
		return false;
	}
	return true;
}
bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	std::string msg = "120";
	_currentTurnAnswers++;
	bool correct = false;
	if (_questions[_currQuestionIndex]->getCorrectAnswerIndex() == answerNo)
	{
		_results.at(user->getUsername())++;
		correct = true;
		msg += "1";
	}
	else
	{
		msg += "0";
	}
	if (answerNo == 5)
	{
		_db.addAnswerToPlayer(_id, user->getUsername(), _questions[_currQuestionIndex]->getId(), "" , correct, time);
	}
	else
	{
		_db.addAnswerToPlayer(_id, user->getUsername(), _questions[_currQuestionIndex]->getId(), _questions[_currQuestionIndex]->getAnswers()[answerNo], correct, time);
	}
	user->send(msg);
	handleNextTurn();
	if (_currentTurnAnswers == _questions_no)
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool Game::leaveGame(User* currUser)
{
	int size = _users.size();
	for (int i = 0; i < size; i++)
	{
		if (_users[i] == currUser)
		{
			_users.erase(_users.begin() + i);
			handleNextTurn();
			return true;
		}
	}
	if (_users.size() == 0)
		return false;
	return true;
}
int Game::getID()
{
	return _id;
}
bool Game::insertGame_db()
{
	if (_db.insertNewGame() != -1)
		return true;
	return false;
}
void Game::initQuestionsFrom_db()
{
	_questions=_db.initQuestions(_questions_no);
}
std::map<std::string, int> Game::getResults()
{
	return _results;
}
