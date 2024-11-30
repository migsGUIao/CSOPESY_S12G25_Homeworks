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

	this->num_cpu = stoi(tokens[0]); // (1 - 128)
	this->scheduler_type = tokens[1]; // ("fcfs" or "rr")
	this->quantum_cycles = stoi(tokens[2]); // (1 - 2^32) 
	this->batch_process_freq = stoi(tokens[3]); // (1 - 2^32), if 1: new process is created per cycle
	this->min_ins = stoi(tokens[4]); // (1 - 2^32)
	this->max_ins = stoi(tokens[5]); // (1 - 2^32)
	this->delay_per_exec = stoi(tokens[6]); // (0 - 2^32), if 0: each instruction is executed per cycle

	this->isActive = false;
	this->pid = 0;

	cout << "Num_cpu: " << num_cpu << endl
		<< "Schduler: " << scheduler_type << endl
		<< "Quantum Cycles: " << quantum_cycles << endl
		<< "Batch Process Freq: " << batch_process_freq << endl
		<< "Min_ins: " << min_ins << endl
		<< "Max_ins: " << max_ins << endl
		<< "Delay per exec: " << delay_per_exec << endl;
}

void Scheduler::createProcess(string processName)
{
	lock_guard<mutex> lock(mtx);
	shared_ptr<Process> newProcess = make_shared<Process>(processName, getCurrentTime(), pid, min_ins, max_ins);
	processMap[processName] = newProcess;
	processQueue.push(newProcess);
	pid++;

	cv.notify_one();
}

void Scheduler::startSchedule()
{
	this->cpuCycles = 0;
	this->nActiveCores = num_cpu;
	this->isActive = true;
	clock = thread(&Scheduler::cpuClock, this);

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
		this->nActiveCores = 0;
	}
	cv.notify_all();
	cv2.notify_all();

	for (thread& t : worker_threads) {
		t.join();
	}
	clock.join();
	worker_threads.clear();
}

void Scheduler::cpuClock()
{
	while (isActive) {
		if (cpuCycles % batch_process_freq == 0) {
			createProcess("Process" + to_string(pid));
		}


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

			/* Round Robin Exit Condition: */
			/* Exit if scheduler is not active regardless if there are unfinished processes in the cpuQueue */
			/* and processQueue */
			if (!isActive) {
				while (!cpuQueue.empty()) {
					processQueue.push(cpuQueue.front());
					cpuQueue.pop();
				}
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
			if (currentProcess->getCurrLine() != currentProcess->getTotalLine()) {
				while (i < quantum_cycles) {
					cv2.wait(lock);
					if (currentProcess->getCurrLine() == currentProcess->getTotalLine()) {
						break;
					}
					currentProcess->setCurrLine(currentProcess->getCurrLine() + 1);

					if (!isActive) {
						break;
					}
					i++;
				}

				if (currentProcess->getCurrLine() == currentProcess->getTotalLine()) {
					cpuQueue.pop();
				}
				else {
					cpuQueue.push(currentProcess);
					cpuQueue.pop();
				}

				if (!isActive) {
					while (!cpuQueue.empty()) {
						processQueue.push(cpuQueue.front());
						cpuQueue.pop();
					}
					return;
				}
			}
			else {
				cpuQueue.pop();
				break;
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

			/* FCFS Exit Condition: */
			/* Exit if scheduler is not active regardless if there are unfinished processes */
			if (!isActive) {
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
				if (!isActive) {
					return;
				}
			}
		}
	}
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
