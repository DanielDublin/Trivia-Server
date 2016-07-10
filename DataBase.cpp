
#include <iostream>
#include <string>
#include "User.h"
#include "Room.h"
#include "Question.h"
#include "Validator.h"
#include "sqlite3.h"
#include <map>
#include <vector>
#include <queue>
#include "DataBase.h"

static string outsideString = "";
static int outsideNumber = 0;

int callbackExistingUsernameOrAddedUsername(void* notUsed, int argc, char** argv, char** azCol)   //returns if the command was successful (of sql)
{
	if (argv[0] == NULL)
	{
		return -1;
	}

	return 1;
}

int callbackNumber(void* notUsed, int argc, char** argv, char** azCol) //The value that was called back will be stored outside
{
	outsideNumber = 0;

	if (argv[0] == NULL)
	{
		return -1;
	}


	string temp = argv[0];


	outsideNumber = atoi(temp.c_str());

	return 1;
}

int callbackString(void* notUsed, int argc, char** argv, char** azCol) //The value that was called back will be stored outside
{
	outsideString = "";
	if (argv[0] == NULL)
	{
		return -1;
	}

	for (int i = 0; i < argc; i++)
	{
		outsideString += argv[i];
		if (i != argc - 1)
		{
			outsideString += " ";
		}
	}


	return 1;
}


DataBase::DataBase()  //checked
{
	int rc;
	try
	{
		rc = sqlite3_open("trivia.db", &_db);
		if (rc)
		{
			throw sqlite3_errmsg(_db);
		}
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
}
DataBase::DataBase(DataBase *db) //checked
{
	this->_db = db->_db;
	this->_games = db->_games;
	this->_questions = db->_questions;
	this->_users = db->_users;
	

}
DataBase::~DataBase()//checked
{
	try
	{
		int rc = sqlite3_close(_db);
		if (rc)
		{
			throw sqlite3_errmsg(_db);
		}
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
}
std::map<int, Game*> DataBase::getGames()  //checked
{

	return  _games;
}
bool DataBase::isUserExists(string username) //checked
{

	char** err = NULL;
	string a = "SELECT username  FROM t_users WHERE username  = '" + username + "';";
	const char* line = a.c_str();
	int rc2 = sqlite3_exec(_db, line, callbackExistingUsernameOrAddedUsername, NULL, err);

	if ((rc2 == 0 )|| (rc2 == -1))
	{
		return false;
	}

	return true;

}
bool DataBase::addNewUserInDataBase(string username, string password, string email)  // checked
{
	if (!isUserExists(username))
	{
		//adds to the db
		char** err = NULL;

		string a = "INSERT INTO t_users values('" + username + "', '" + password + "', '" + email + "' );";
		const char* line = a.c_str();
		int rc2 = sqlite3_exec(_db, line, callbackExistingUsernameOrAddedUsername, NULL, err);

		if (rc2 == 0)
		{
			return false;
		}
		std::vector<string> temp;
		temp.push_back(password);
		temp.push_back(email);

		std::map <string, std::vector<string>> output;
		output.insert(std::make_pair(username, temp));

		sqlite3_free(err);
		return true;

	}
	return false;
}
bool DataBase::isUserAndPassMatch(string username, string password) // checked
{
	if (isUserExists(username))
	{

		char** err = NULL;
		string a = "SELECT username  FROM t_users WHERE username  = '" + username + "' and password = '" + password + "' ;";
		const char* line = a.c_str();
		int rc2 = sqlite3_exec(_db, line, callbackExistingUsernameOrAddedUsername, NULL, err);

		if ((rc2 == 0) || (rc2 == -1))
		{
			return false;
		}
		sqlite3_free(err);
		return true;

	}


	return false;
}
std::vector<Question*> DataBase::initQuestions(int questionsNo) //checked + confirmed randomizations of both questions and answers
{

	std::queue<int> randomNumbers;
	std::queue<int> tempQueue;
		randomNumbers.push(-1);

	std::vector<Question*> empty;

		for (int i = 0; i < questionsNo; i++)
		{
			int spot = 0;

			// randomizing the questions
			int size1 = randomNumbers.size();
			if (size1 == 1)
			{
				spot = rand() % 9 + 1;
				randomNumbers.push(spot);
			}
			else
			{
				bool found = false;
				do
				{
					found = false;
					spot = rand() % 9 + 1;
					while (randomNumbers.size() != 0)
					{
						int x = randomNumbers.front();
						randomNumbers.pop();
						if (spot == x)
						{
							found = true;
						}

						tempQueue.push(x);
					}

					while (tempQueue.size() != 0)
					{
						randomNumbers.push(tempQueue.front());
						tempQueue.pop();
					}
				} while (found);
				randomNumbers.push(spot);
			}
			// end of randomizing questions

			Question* q = NULL;
			char** err = 0;

			std::string temp1 = "SELECT question FROM t_questions WHERE question_id = '" + std::to_string(spot) + "';";
			const char* qs = { temp1.c_str() };
			std::string temp2 = "SELECT correct_ans FROM t_questions WHERE question_id  = " + std::to_string(spot) + ';';
			const char* cor_ans = { temp2.c_str() };
			std::string temp3 = "SELECT ans2 FROM t_questions WHERE question_id  = " + std::to_string(spot) + ';';
			const char* ans2 = { temp3.c_str() };
			std::string temp4 = "SELECT ans3 FROM t_questions WHERE question_id  = " + std::to_string(spot) + ';';
			const char* ans3 = { temp4.c_str() };
			std::string temp5 = "SELECT ans4 FROM t_questions WHERE question_id  = " + std::to_string(spot) + ';';
			const char* ans4 = { temp5.c_str() };


			int rc2 = sqlite3_exec(_db, qs, callbackString, NULL, err);
			std::string question = outsideString;
			err = 0;

			rc2 = sqlite3_exec(_db, cor_ans, callbackString, NULL, err);
			std::string correct_ans = outsideString;
			err = 0;
			rc2 = sqlite3_exec(_db, ans2, callbackString, NULL, err);
			std::string answer2 = outsideString;
			err = 0;
			rc2 = sqlite3_exec(_db, ans3, callbackString, NULL, err);
			std::string answer3 = outsideString;
			err = 0;
			rc2 = sqlite3_exec(_db, ans4, callbackString, NULL, err);
			std::string answer4 = outsideString;
			err = 0;



			std::string  questions[] = { correct_ans, answer2, answer3, answer4 };

			q = new Question(spot, question, 0, questions);
			empty.push_back(q);
			sqlite3_free(err);
		}
	
		return empty;
	
}
std::vector<string> DataBase::getBestScores()  //checked
{
	

	std::map<string, int> names;
	int max = 0, max2 = 0, max3 = 0;
	string allScores = "", temp1 = "", temp2 = "";;
	std::vector<string> output;
	char** err = 0;


	sqlite3_stmt *stmt;

	const char* allScoresSums = "select username, SUM(is_correct) as TotalSums "
		"from t_players_answers group by username "
		"having sum(is_correct) order by (TotalSums) desc LIMIT 3;";


	int rc = sqlite3_prepare_v2(_db, allScoresSums, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error: %s!\n", sqlite3_errmsg(_db));
		return output;
	}

	for (;;)
	{
		rc = sqlite3_step(stmt);
		if (rc == SQLITE_DONE)
			break;
		if (rc != SQLITE_ROW)
		{
			printf("error: %s!\n", sqlite3_errmsg(_db));
			break;
		}


		const char *text = (const char *)sqlite3_column_text(stmt, 0);
		string line = text ? text : "";
		line += " " + std::to_string(sqlite3_column_int(stmt, 1));
		output.push_back(line);
	}

	sqlite3_finalize(stmt);
	sqlite3_free(err);




	return output;
}
std::vector<string> DataBase::getPersonalStatus(string username) // checked
{

	std::vector<string> empty;
	if (isUserExists(username))
	{
		char** err = 0;
	
		string output = "";
		string line1 = "select count (distinct game_id) from t_players_answers where username = '" + username + "';";
		string line2 = "select count ( is_correct) from t_players_answers where username = '" + username + "' and is_correct = 1;";
		string line3 = "select count ( is_correct) from t_players_answers where username = '" + username + "' and is_correct = 0;";
		string line4 = "select avg( answer_time) from t_players_answers where username = '" + username + "';";
		const char* c_line1 = line1.c_str();
		const char* c_line2 = line2.c_str();
		const char* c_line3 = line3.c_str();
		const char* c_line4 = line4.c_str();
		int rc2 = sqlite3_exec(_db, c_line1, callbackString, NULL, err);
		if (rc2 != -1)
		{
			output = outsideString;
			empty.push_back(output);
			output = "";
		}
		else
		{
			empty.push_back("");
		}
		err = 0;
		rc2 = sqlite3_exec(_db, c_line2, callbackString, NULL, err);
		err = 0;
		if (rc2 != -1)
		{
			output = outsideString;
			empty.push_back(output);
			output = "";
		}
		else
		{
			empty.push_back("");
		}
		rc2 = sqlite3_exec(_db, c_line3, callbackString, NULL, err);
		if (rc2 != -1)
		{
			output = outsideString;
			empty.push_back(output);
			output = "";
		}
		else
		{
			empty.push_back("");
		}
		err = 0;
		rc2 = sqlite3_exec(_db, c_line4, callbackString, NULL, err);
		if (rc2 != -1)
		{
			output = outsideString;
			empty.push_back(output);
			output = "";
		}
		else
		{
			empty.push_back("");
		}
		err = 0;


		sqlite3_free(err);
	}

	return empty;
}
int DataBase::insertNewGame() // checked
{

	char**zErrMsg = 0;
	char**zErrMsg2 = 0;

	int id = -1;
	string a = "INSERT INTO t_games values(null, 0, (DateTime('now')) , null);";
	//this part (first 0) was supposed to be a function to print out the success
	int rc1 = sqlite3_exec(_db, a.c_str(), 0, 0, zErrMsg);
	if (rc1 != SQLITE_OK)
	{
		std::cout << "problem in insertNewGame first if statement" << std::endl;
		sqlite3_free(zErrMsg);
		return id;
	}
	a = "select game_id from t_games where rowid = (select max(rowid) from t_games); ";
	const char* b = a.c_str();
	int rc2 = sqlite3_exec(_db, a.c_str(), callbackNumber, 0, zErrMsg2);
	id = outsideNumber;
	sqlite3_free(zErrMsg2);

	

	return id;
}
bool DataBase::updateGameStatus(int status)//checked
{
	const char* max = "select Max(game_id) from t_games;";
	char** zErrMsg = 0;
	int size = sqlite3_exec(_db, max, callbackNumber, NULL, zErrMsg);

	if (size != -1)
	{

		if ((status > 0) && (status <= outsideNumber))
		{



			string a = "UPDATE t_games SET status = 1, end_time = ( select DateTime('now') )  where game_id = " + std::to_string(status) + ";";
			const char* line = a.c_str();
			zErrMsg = 0;
			int rc2 = sqlite3_exec(_db, line, 0, 0, zErrMsg);



		
			if (rc2 != SQLITE_OK)
			{
				std::cout << "problem in updateGameStatus first if statement" << std::endl;
				sqlite3_free(zErrMsg);
				return false;
			}
			return true;
		}
	}
	return false;
}
bool DataBase::addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime)//checked
{
	char* zErrMsg = 0;
	int correct = 0;
	if (isCorrect)
	{
		correct++;
	}
	string a = "INSERT INTO t_players_answers values( '" + std::to_string(gameId) + "' , '" + username + "' , '" + std::to_string(questionId) + "' , '" + answer + "' , '" + std::to_string(correct) + "' , '" + std::to_string(answerTime) + "' );";
	int rc = sqlite3_exec(_db, a.c_str(), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		std::cout << "problem in addAnswerToPlayer first if statement" << std::endl;
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}
int DataBase::callbackCount(void*, int, char**, char**)
{
	return -1;
}
int DataBase::callbackQuestions(void*, int, char**, char**)
{
	return -1;
}
int DataBase::callbackBestScores(void*, int, char**, char**)
{
	return -1;
}
int DataBase::callbackPersonalStatus(void*, int, char**, char**)
{
	return -1;
}




bool DataBase::updateGameStatus(Game* gm) //checked
{
	return this->updateGameStatus(gm->getID());

}


/**
sqlite3_stmt *stmt;
std::map<int, Game*> games;
const char* allGames = "select * from t_games;";
Game* g = 0;

int rc = sqlite3_prepare_v2(_db, allGames, -1, &stmt, NULL);
if (rc != SQLITE_OK) {
	printf("error: %s!\n", sqlite3_errmsg(_db));
	return games;
}

for (;;)
{
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE)
		break;
	if (rc != SQLITE_ROW)
	{
		printf("error: %s!\n", sqlite3_errmsg(_db));
		break;
	}


	int id = sqlite3_column_int(stmt, 0);
	int status = sqlite3_column_int(stmt, 1);
	const char *text1 = (const char *)sqlite3_column_text(stmt, 2);
	const char *text2 = (const char *)sqlite3_column_text(stmt, 3);

	string line1 = text1 ? text1 : "";
	string line2 = text2 ? text2 : "";

	g = new Game();

	output.push_back(line);
}

**/