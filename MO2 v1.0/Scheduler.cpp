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
	this->max_overall_mem = stoi(tokens[7]);
	this->mem_per_frame = stoi(tokens[8]);
	this->min_mem_per_proc = stoi(tokens[9]);
	this->max_mem_per_proc = stoi(tokens[10]);

	
	if (max_overall_mem != mem_per_frame) {
		this->memoryAllocator = make_unique<PagingAllocator>(max_overall_mem, mem_per_frame);
	} else {
		/*this->memoryAllocator = make_unique<FlatMemoryAllocator>();*/
		// TO DO: Implement FlatMemoryAllocator
	}
	

	this->isActive = false;
	this->pid = 0;
	this->nActiveCores = 0;
	this->completed_processes = 0;
}

void Scheduler::createProcess(string processName)
{
	lock_guard<mutex> lock(mtx);
	int rmax_ins = randomNumberGenerator(min_ins, max_ins);
	int rmem_per_proc = randomNumberGenerator(min_mem_per_proc, max_mem_per_proc);

	shared_ptr<Process> newProcess = make_shared<Process>(processName, getCurrentTime(), pid, rmax_ins, rmem_per_proc);
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
	this->activeCycles = 0;
	this->idleCycles = 0;
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
		if (cpuCycles % batch_process_freq == 0 && isActive) {
			createProcess("Process" + to_string(pid));
		}


		if (cpuCycles % (delay_per_exec + 1) == 0) {
			cv2.notify_all();
		}


		if (nActiveCores == 0) {
			idleCycles++;
		}
		else {
			activeCycles++;
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
			cv3.wait(lock, [&cpuQueue, this] {return memoryAllocator->allocate(cpuQueue.front()); });
			cpuQueue.front()->setStatus("RUNNING");
		}

		{
			unique_lock<mutex> lock(mtx);
			shared_ptr<Process> currentProcess = cpuQueue.front();
			currentProcess->setCoreNumber(cpuID);
			nActiveCores++;

			/*currentProcess->setStatus("RUNNING")*/;

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
				memoryAllocator->deallocate(currentProcess);
				currentProcess->setStatus("COMPLETED");
				cpuQueue.pop();
				completed_processes++;
			}
			else {
				currentProcess->setStatus("WAITING");
				cpuQueue.push(currentProcess);
				cpuQueue.pop();
			}

			cv3.notify_one();
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
			cv3.wait(lock, [&currentProcess, this] {return memoryAllocator->allocate(currentProcess); });
		}
		
		{
			unique_lock<mutex> lock(mtx);
			nActiveCores++;
			currentProcess->setCoreNumber(cpuID);
			currentProcess->setStatus("RUNNING");

			while (currentProcess->getCurrLine() < currentProcess->getTotalLine()) {
				cv2.wait(lock);
				currentProcess->setCurrLine(currentProcess->getCurrLine() + 1);
			}
			completed_processes++;
			currentProcess->setStatus("COMPLETED");
			memoryAllocator->deallocate(currentProcess);
			cv3.notify_one();
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

int Scheduler::getActiveCores()
{
	return this->nActiveCores;
}

string Scheduler::getProcess_smiInfo()
{
	string info;
	map<int, shared_ptr<Process>> memoryMap = memoryAllocator->getAllocationMap();

	if (dynamic_pointer_cast<PagingAllocator>(memoryAllocator)) {
		
		int mem_used = memoryAllocator->getTotalMemoryUsed();
		float cpu_util = static_cast<float>(nActiveCores * 100 / num_cpu);
		float mem_util = static_cast<float>(mem_used * 100 / max_overall_mem);

		info += to_string(cpu_util) + " ";
		info += to_string(mem_used) + " ";
		info += to_string(max_overall_mem) + " ";
		info += to_string(mem_util) + " ";


		for (auto lt = memoryMap.begin(); lt != memoryMap.end(); lt++) {
			info += lt->second->getName() + " ";
			info += to_string(lt->second->getTotalMemory()) + " ";
		}
	}
	else if (dynamic_pointer_cast<FlatMemoryAllocator>(memoryAllocator)) {
		// TO DO: Implement
	}

	return info;
}

string Scheduler::getVmstatInfo()
{
	string info;

	if (dynamic_pointer_cast<PagingAllocator>(memoryAllocator)) {
		info += to_string(max_overall_mem) + " ";
		info += to_string(memoryAllocator->getTotalMemoryUsed()) + " ";
		info += to_string(max_overall_mem - memoryAllocator->getTotalMemoryUsed()) + " ";
		info += to_string(idleCycles) + " ";
		info += to_string(activeCycles) + " ";
		info += to_string(cpuCycles) + " ";
		info += to_string(dynamic_pointer_cast<PagingAllocator>(memoryAllocator)->getNumPagesIn()) + " ";
		info += to_string(dynamic_pointer_cast<PagingAllocator>(memoryAllocator)->getNumPagesOut()) + " ";

		info += to_string(completed_processes);
	}
	else if (dynamic_pointer_cast<FlatMemoryAllocator>(memoryAllocator)){
		// TO DO: Implement
	}

	return info;
}