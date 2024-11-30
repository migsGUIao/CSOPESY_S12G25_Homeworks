#pragma once
#include <fstream>
#include <sstream>

#include <queue>
#include <unordered_map>
#include <vector>
#include <memory>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>
#include <ctime>
#include <iomanip>

#include "Process.h"
#include <iostream>
using namespace std;

class Scheduler
{
private:
	int num_cpu;
	string scheduler_type;
	int quantum_cycles;
	int batch_process_freq;
	int min_ins;
	int max_ins;
	int delay_per_exec;

	int pid;
	bool isActive;
	unordered_map<string, shared_ptr<Process>> processMap;
	queue<shared_ptr<Process>> processQueue;

	thread clock;
	vector<thread> worker_threads;
	mutex mtx;
	condition_variable cv;

public:
	Scheduler(string);
	void createProcess(string);
	void startSchedule();
	void stopSchedule();
	void cpuClock();
	void roundRobin(int);
	void fcfs(int);
	string getCurrentTime();
	unordered_map<string, shared_ptr<Process>> getProcessMap();
	int getNumCpu();
	bool getStatus();
};



