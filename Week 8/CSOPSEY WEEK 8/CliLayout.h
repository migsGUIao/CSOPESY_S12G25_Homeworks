#pragma once
#include <iostream>
#include <memory>
#include <map>
#include <fstream>
#include <iomanip>
#include "Process.h"
#include "Colors.h"

using namespace std;

class CliLayout
{
public:
	void clearScreen();
	string mainScreen();
	void screen_r_Screen(shared_ptr<Process>);
	void screen_ls_Screen(map<string, shared_ptr<Process>>, int, int);
	bool reportutil_Screen(map<string, shared_ptr<Process>>, int, int);
};


