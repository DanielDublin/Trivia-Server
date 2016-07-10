#pragma once
#include <iostream>

class Validator
{
public:

	Validator(std::string user, std::string pass);
	bool isPasswordValid();
	bool isUsernameValid();


private:
	std::string username;
	std::string password;
};