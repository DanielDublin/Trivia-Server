#include <iostream>
#include <string>
#include "Validator.h"
using std::string;
Validator::Validator(string user, string pass)
{
	username = user;
	password = pass;
}
bool Validator::isPasswordValid()
{
	if (password.length() > 4)
	{
		int didgit = 0, capital = 0, regular = 0;
		for (int i = 0; i < password.length(); i++)
		{
			if (password[i] == ' '){ return false; }
			if ((password[i] >= '0') && (password[i] <= '9')){ didgit++; }
			else if ((password[i] >= 'a') && (password[i] <= 'z')){ regular++; }
			else if ((password[i] >= 'A') && (password[i] <= 'Z')) { capital++; }
		}
		if ((didgit == 0) || (capital == 0) || (regular == 0)){ return false; }
		else { return true; }
	}
	return false;
}
bool Validator::isUsernameValid()
{
	bool ok = false;
	if (username.length() == 0) { return false; }
	if (((username[0] >= 'A') && (username[0] <= 'Z')) || ((username[0] >= 'a') && (username[0] <= 'z'))) { ok = true; }
	if (!ok) { return false; }
	for (int i = 0; i < username.length(); i++) if (username[i] == ' ') { return false; }
	return true;
}