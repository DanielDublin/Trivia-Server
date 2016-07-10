#include <stdio.h>
#include <string>
#include "Question.h"
#include <time.h>

using namespace std;

Question::Question(int id, string question, int correctAnswerIndex, string answers[4])
{
	string tempAnswer = "";
	this->id = id;
	this->question = question;
	srand(time(NULL));
	int size = (rand() % 12) + 8;
	for (int i = 0; i < 9; i++)
	{
		int fromSpot = (rand() % 4);
		int toSpot = (rand() % 4);
		if (fromSpot != toSpot)
		{
			if (correctAnswerIndex == fromSpot)
			{
				correctAnswerIndex = toSpot;
			}
			else if (correctAnswerIndex == toSpot)
			{
				correctAnswerIndex = fromSpot;
			}
			tempAnswer = answers[toSpot];
			answers[toSpot] = answers[fromSpot];
			answers[fromSpot] = tempAnswer;
		}
	}
	for (int i = 0; i < 4; i++) { this->answers[i] = answers[i]; }
	this->correctAnswerIndex = correctAnswerIndex;
}

string* Question::getAnswers()
{
	return this->answers;
}
int Question::getCorrectAnswerIndex()
{
	return this->correctAnswerIndex;
}

int Question::getId()
{
	return this->id;
}

string Question::getQuestion()
{
	return this->question;

}







