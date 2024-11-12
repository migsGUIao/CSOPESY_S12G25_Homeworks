#include "Scheduler.h"

Scheduler::Scheduler(string filename) {
	string filebuffer;
	ifstream file(filename);
	vector<string> tokens;
	int i = 0;


	while (getline(file, filebuffer)) {
		stringstream stream(filebuffer);
		string strbuffer;

		while (stream >> strbuffer) {
			if (i % 2 == 1) {
				tokens.push_back(strbuffer);
			}
			i++;
		}
	}

	file.close();

	// Program does not check for invalid configurations:
	this->num_cpu = stoi(tokens[0]); // (1 - 128)
	this->scheduler_type = tokens[1]; // ("fcfs" or "rr")
	this->quantum_cycles = stoi(tokens[2]); // (1 - 2^32) 
	this->batch_process_freq = stoi(tokens[3]); // (1 - 2^32), if 1: new process is created per cycle
	this->min_ins = stoi(tokens[4]); // (1 - 2^32)
	this->max_ins = stoi(tokens[5]); // (1 - 2^32)
	this->delay_per_exec = stoi(tokens[6]); // (0 - 2^32), if 0: each instruction is executed per cycle

	// Set other fields:
	this->isActive = false;
	this->pid = 0;
	this->nActiveCores = 0;
}

void Scheduler::createProcess(string processName)
{
	lock_guard<mutex> lock(mtx); 
	
	
	int random = randomNumberGenerator(min_ins, max_ins);
	shared_ptr<Process> newProcess = make_shared<Process>(processName, getCurrentTime(), pid, 1, random);
	processMap[processName] = newProcess;
	processQueue.push(newProcess);
	pid++;

	cv.notify_one();
}

void Scheduler::startSchedule()
{
	if (nActiveCores == 0 && !worker_threads.empty()) {
		worker_threads.clear();
	}

	this->cpuCycles = 0;
	this->nActiveCores = num_cpu;
	this->isActive = true;
	if (!clock.joinable()) {
		clock = thread(&Scheduler::cpuClock, this);
	}


	if (scheduler_type == "\"fcfs\"") {
		for (int i = 0; i < num_cpu; i++) {
			worker_threads.emplace_back(&Scheduler::fcfs, this, i);
		}
	}
	else {
		for (int i = 0; i < num_cpu; i++) {
			worker_threads.emplace_back(&Scheduler::roundRobin, this, i);
		}
	}

}

void Scheduler::stopSchedule()
{
	{
		unique_lock<mutex> lock(mtx);
		this->isActive = false;
	}
	cv.notify_all();
	cv2.notify_all();

	for (thread& t : worker_threads) {
		t.detach();
	}

	clock.detach();

}

void Scheduler::cpuClock()
{
	while (true) {
		// Create new process every batch_process_freq and while scheduler is active
		if (cpuCycles % batch_process_freq == 0 && isActive) {
			createProcess("Process" + to_string(pid));
		}

		// Signal signal to every cpu (thread) to run a line of instruction for all processes they handle
		if (cpuCycles % (delay_per_exec + 1) == 0) {
			cv2.notify_all();
		}

		cpuCycles++;
		this_thread::sleep_for(chrono::milliseconds(25));
	}
}

void Scheduler::roundRobin(int cpuID)
{
	queue<shared_ptr<Process>> cpuQueue;

	while (true) {
		{
			unique_lock<mutex> lock(mtx);
			nActiveCores--;
			cv.wait(lock, [&cpuQueue, this] { return !isActive || !processQueue.empty() || !cpuQueue.empty(); });

			if (!isActive && cpuQueue.empty() && processQueue.empty()) {
				return;
			}


			if (!processQueue.empty()) {
				cpuQueue.push(processQueue.front());
				processQueue.pop();
			}

			if (cpuQueue.empty()) {
				continue;
			}
		}

		{
			unique_lock<mutex> lock(mtx);
			shared_ptr<Process> currentProcess = cpuQueue.front();
			currentProcess->setCoreNumber(cpuID);
			nActiveCores++;

			int i = 0;
			while (i < quantum_cycles) {
				cv2.wait(lock);
				if (currentProcess->getCurrLine() == currentProcess->getTotalLine()) {
					break;
				}
				currentProcess->setCurrLine(currentProcess->getCurrLine() + 1);
				i++;
			}

			if (currentProcess->getCurrLine() == currentProcess->getTotalLine()) {
				cpuQueue.pop();
			}
			else {
				cpuQueue.push(currentProcess);
				cpuQueue.pop();
			}
		}
	}
}

void Scheduler::fcfs(int cpuID)
{
	shared_ptr<Process> currentProcess;
	while (true) {
		{
			unique_lock<mutex> lock(mtx);
			nActiveCores--;
			cv.wait(lock, [this] { return !isActive || !processQueue.empty(); });

			if (!isActive && processQueue.empty()) {
				return;
			}

			if (!processQueue.empty()) {
				currentProcess = processQueue.front();
				processQueue.pop();
			}
			else {
				continue;
			}
		}

		{
			unique_lock<mutex> lock(mtx);
			nActiveCores++;
			currentProcess->setCoreNumber(cpuID);
			while (currentProcess->getCurrLine() < currentProcess->getTotalLine()) {
				cv2.wait(lock);
				currentProcess->setCurrLine(currentProcess->getCurrLine() + 1);
			}
		}
	}
}

int Scheduler::randomNumberGenerator(int min, int max)
{
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 eng(rd()); // Seed the generator

	// Create a distribution in the range [min, max]
	std::uniform_int_distribution<> distr(min, max); // Define the range

	// Generate and print a random number
	int random = distr(eng);

	return random;
}

string Scheduler::getCurrentTime()
{
	auto now = chrono::system_clock::now();
	time_t now_c = chrono::system_clock::to_time_t(now);

	// Convert to local time
	tm local_time;

#ifdef _WIN32
	localtime_s(&local_time, &now_c); // Use localtime_s on Windows
#else
	localtime_r(&now_c, &local_time);  // Use localtime_r on POSIX systems
#endif

	// Create a string stream to format the time
	ostringstream oss;
	oss << std::put_time(&local_time, "(%m/%d/%Y  %I:%M:%S %p)");

	return oss.str();
}

map<string, shared_ptr<Process>> Scheduler::getProcessMap()
{
	return this->processMap;
}

int Scheduler::getNumCpu()
{
	return this->num_cpu;
}

bool Scheduler::getStatus()
{
	return this->isActive;
}

int Scheduler::getActiveCores()
{
	return this->nActiveCores;
}