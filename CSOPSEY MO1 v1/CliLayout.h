#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
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
	void screen_ls_Screen(unordered_map<string, shared_ptr<Process>>, int, bool);
	bool reportutil_Screen(unordered_map<string, shared_ptr<Process>>, int, bool);
};


