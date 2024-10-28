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

	this->num_cpu = stoi(tokens[0]);
	this->scheduler_type = tokens[1];
	this->quantum_cycles = stoi(tokens[2]);
	this->batch_process_freq = stoi(tokens[3]);
	this->min_ins = stoi(tokens[4]);
	this->max_ins = stoi(tokens[5]);
	this->delay_per_exec = stoi(tokens[6]);

	this->isActive = false;
	this->pid = 0;

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
	}
	cv.notify_all();

	for (thread& t : worker_threads) {
		t.join();
	}
	clock.join();
	worker_threads.clear();
}

void Scheduler::cpuClock()
{
	while (isActive) {
		for (int i = 0; i < batch_process_freq; i++) {
			createProcess("Process" + to_string(pid));
		}
		this_thread::sleep_for(chrono::milliseconds(500));
	}

}

void Scheduler::roundRobin(int cpuID)
{
	queue<shared_ptr<Process>> cpuQueue;

	while (true) {
		{
			unique_lock<mutex> lock(mtx);
			cv.wait(lock, [this] { return !isActive || !processQueue.empty(); });

			/* Round Robin Exit Condition 1: */
			/* Exit if and only if scheduler is not active and processQueue and cpuQueue is empty */
			/* Warning: Program may freeze for a long time while waiting for all processes to be finished */
			if (!isActive && processQueue.empty() && cpuQueue.empty()) {
				return;
			}

			/* Round Robin Exit Condition 2: */
			/* Exit if scheduler is not active */
			/* Note: Much more stable than Round Robin Exit Condition 1 */
			//if (!isActive) {
			//	while (!cpuQueue.empty()) {
			//		processQueue.push(cpuQueue.front());
			//		cpuQueue.pop();
			//	}
			//	return;
			//}
			//

			if (!processQueue.empty() && isActive) {
				cpuQueue.push(processQueue.front());
				processQueue.pop();
			}

			if (cpuQueue.empty() || !isActive) {
				continue;
			}
		}

		{
			lock_guard<mutex> lock(mtx);
			cpuQueue.front()->setCoreNumber(cpuID);

			if (cpuQueue.front()->getCurrLine() + quantum_cycles < cpuQueue.front()->getTotalLine()) {
				cpuQueue.front()->setCurrLine(cpuQueue.front()->getCurrLine() + quantum_cycles);
				cpuQueue.push(cpuQueue.front());
				cpuQueue.pop();
			}
			else {
				cpuQueue.front()->setCurrLine(cpuQueue.front()->getTotalLine());
				cpuQueue.pop();
			}
		}

		this_thread::sleep_for(chrono::milliseconds(delay_per_exec));
	}
}

void Scheduler::fcfs(int cpuID)
{
	shared_ptr<Process> currentProcess;
	while (true) {
		{
			unique_lock<mutex> lock(mtx);
			cv.wait(lock, [this] { return !isActive || !processQueue.empty(); });

			/* FCFS Exit Condition 1: */
			/* Exit if and only if scheduler is not active and processQueue is empty */
			if (!isActive && processQueue.empty()) {
				return;
			}

			/* FCFS Exit Condition 2: */
			/* Exit if scheduler is not active */
			//if (!isActive) {
			//	return;
			//}

			if (!processQueue.empty()) {
				currentProcess = processQueue.front();
				processQueue.pop();
			}
			else {
				continue;
			}
		}

		{
			lock_guard<mutex> lock(mtx);
			currentProcess->setCoreNumber(cpuID);
			while (currentProcess->getCurrLine() < currentProcess->getTotalLine()) {
				currentProcess->setCurrLine(currentProcess->getCurrLine() + 1);
			}
		}

		this_thread::sleep_for(chrono::milliseconds(delay_per_exec));
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

unordered_map<string, shared_ptr<Process>> Scheduler::getProcessMap()
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
