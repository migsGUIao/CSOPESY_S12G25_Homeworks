#include "Process.h"

Process::Process(string name, string timeCreated, int pid, int totalLine, int totalMemory)
{
	this->name = name;
	this->timeCreated = timeCreated;
	this->pid = pid;
	this->currLine = 1;
	this->totalLine = totalLine;
	this->coreNumber = -1;
	this->totalMemory = totalMemory;
	this->status = "WAITING";
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

int Process::getTotalMemory()
{
	return this->totalMemory;
}

string Process::getStatus()
{
	return this->status;
}

void Process::setCurrLine(int currLine)
{
	this->currLine = currLine;
}

void Process::setCoreNumber(int coreNumber)
{
	this->coreNumber = coreNumber;
}

void Process::setStatus(string status)
{
	this->status = status;
}
