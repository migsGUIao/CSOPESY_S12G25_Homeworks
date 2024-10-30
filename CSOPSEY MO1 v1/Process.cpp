#include "Process.h"

Process::Process(string name, string timeCreated, int pid, int currLine, int totalLine)
{
	this->name = name;
	this->timeCreated = timeCreated;
	this->pid = pid;
	this->currLine = currLine;
	this->totalLine = totalLine;
	this->coreNumber = -1;
}

string Process::getName()
{
	return this->name;
}

string Process::getTimeCreated()
{
	return this->timeCreated;
}

int Process::getPid()
{
	return this->pid;
}

int Process::getCurrLine()
{
	return this->currLine;
}

int Process::getTotalLine()
{
	return this->totalLine;
}

int Process::getCoreNumber()
{
	return this->coreNumber;
}

void Process::setCurrLine(int currLine)
{
	this->currLine = currLine;
}

void Process::setCoreNumber(int coreNumber)
{
	this->coreNumber = coreNumber;
}