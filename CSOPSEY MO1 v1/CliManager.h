#pragma once
#include <sstream>

#include "Colors.h"
#include "CliLayout.h"
#include "Process.h"
#include "Scheduler.h"

using namespace std;

class CliManager
{
private:
	CliLayout layout;
	shared_ptr<Scheduler> scheduler;
	unordered_map<string, shared_ptr<Process>> processes;

public:
	CliManager();
	vector<string> tokenizer(string);
	void screenCommand(vector<string>);
};


