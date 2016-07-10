#pragma once
#include <iostream>
#include <string>

class Question
{
public:
	Question(int id, std::string question, int correctAnswerIndex, std::string answers[4]);
	std::string getQuestion();
	std::string* getAnswers();
	int getCorrectAnswerIndex();
	int getId();
private:
	int id;
	std::string question;
	int correctAnswerIndex;
	std::string answers[4];


};