#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include "sqlite3.h"

using namespace std;




int callbackExistingUsername(void* notUsed, int argc, char** argv, char** azCol)
{
	if (argc == 0)
	{
		return -1;
	}

	return 1;
}



bool isUserExists(string username)
{

	sqlite3* db;
	int rc = sqlite3_open("trivia.db", &db);

	char** err;
	string a = "SELECT username  FROM t_users WHERE username  = " + username;
	const char* line = a.c_str();
	int rc = sqlite3_exec(db, line, callbackExistingUsername, NULL, err);

	if (rc == -1)
	{
		return false;
	}

	return true;
}




void main()
{


	cout << isUserExists() << endl;
	cout << isUserExists() << endl;

}