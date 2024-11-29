#pragma once
#include <string>

using namespace std;

class Process
{
private:
	string name;
	string timeCreated;
	int pid;
	int currLine;
	int totalLine;
	int coreNumber;

	int totalMemory;
	string status;

public:
	Process(string, string, int, int, int);

	string getName();
	string getTimeCreated();
	int getPid();
	int getCurrLine();
	int getTotalLine();
	int getCoreNumber();
	int getTotalMemory();
	string getStatus();

	void setCurrLine(int); 
	void setCoreNumber(int);
	void setStatus(string);
};