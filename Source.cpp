#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>


using namespace std;
	

string has(string username, map <string, std::vector<string>> _users)
{
	auto search = _users.find(username);
	if (search != _users.end())
	{
		return "found";
	}
	return "wasnt found";

}

void main1()
{
	map <string, std::vector<string>> _users;

	
	for (int i = 0; i< 5; i++)
	{
		string username = "";
		string email = "";
		cout << endl << "enter username ";
		cin >> username;
		cout << endl<< "enter email ";
		cin >> email;
		vector <string> personalInfo;
		personalInfo.push_back(email);

		_users.insert(make_pair(username, personalInfo));
	}

	for (int i = 0; i < 5; i++)
	{
		string username = "";
		cout << "enter username ";
		cin >> username;
		string a = has(username, _users);
		cout << a << endl;
	}


}