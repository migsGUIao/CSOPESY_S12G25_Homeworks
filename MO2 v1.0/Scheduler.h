#pragma once
#include <fstream>
#include <sstream>
#include <cstdint>

#include <queue>
#include <map>
#include <vector>
#include <memory>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>
#include <ctime>
#include <iomanip>

#include <memory>
#include <random>

#include "Process.h"
#include "MemoryAllocator.h"
#include "FlatMemoryAllocator.cpp"
#include "PagingAllocator.cpp"
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
	uint32_t cpuCycles;
	uint32_t activeCycles;
	uint32_t idleCycles;
	int nActiveCores;
	bool isActive;
	map<string, shared_ptr<Process>> processMap;
	queue<shared_ptr<Process>> processQueue;

	thread clock;
	vector<thread> worker_threads;
	mutex mtx;
	condition_variable cv;
	condition_variable cv2;
	condition_variable cv3;

	int max_overall_mem;
	int mem_per_frame;
	int min_mem_per_proc;
	int max_mem_per_proc;
	shared_ptr<MemoryAllocator> memoryAllocator; 

	int completed_processes;

public:
	Scheduler(string);
	void createProcess(string);
	void startSchedule();
	void stopSchedule();
	void cpuClock();
	void roundRobin(int);
	void fcfs(int);
	int randomNumberGenerator(int, int);

	string getCurrentTime();
	map<string, shared_ptr<Process>> getProcessMap();
	int getNumCpu();
	int getActiveCores();
	string getProcess_smiInfo();
	string getVmstatInfo();
};