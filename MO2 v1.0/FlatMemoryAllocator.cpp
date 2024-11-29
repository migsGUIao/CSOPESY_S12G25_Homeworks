#include "MemoryAllocator.h"

class FlatMemoryAllocator : public MemoryAllocator {

private:
	int mem_per_block;
	vector<char> memory;

	bool canAllocate(int required_blocks, int index) override {
		// Check memory vector if block is unoccupied
		for (int i = 0; i < required_blocks; i++) {
			if (memory.at(i + index) == 'O') { // Meaning memory is occupied
				return false;
			}
		}

		return true;
	}

	void allocateAt(shared_ptr<Process> process, int required_blocks, int index)  override {
		// Allocate memory to the process and change block status to 'O'
		for (int i = 0; i < required_blocks; i++) {
			memory.at(i + index) = 'O'; // Meaning memory is occupied
		}

		// Record the memory to the allocationMap wherein key is equal to the address where 
		// the memoryis stored
		allocationMap[index * mem_per_block] = process;
	}

	void checkBackingStore(shared_ptr<Process> process) override {
		string filename = "backing_store/" + process->getName() + ".txt";

		// Delete the backing store of the process if it exists
		if (filesystem::exists(filename)) {
			filesystem::remove(filename);
		}
	}

	void backingStore() override {

		// Remove process that is currently on standby in memory
		shared_ptr<Process> process;

		// Find process in allocation map
		for (auto it = allocationMap.begin(); it != allocationMap.end(); it++) {
			if (it->second->getStatus() != "RUNNING") {
				process = it->second;
				break;
			}
		}

		// No process found, exit function
		if (!process) {
			return;
		}

		// Deallocate process in the memory
		deallocate(process);

		// Store to backing store
		string subfolder = "backing_store";

		ofstream file(subfolder + "/" + process->getName() + ".txt");
		file << "Sample Backing store of " << process->getName() << endl;
		file << "-Name: " << process->getName() << endl;
		file << "-Time Created: " << process->getTimeCreated() << endl;
		file << "-Pid: " << process->getPid() << endl;
		file << "-Current line: " << process->getCurrLine() << endl;
		file << "-Total line: " << process->getTotalLine() << endl;
		file << "-Assigned cpu id: " << process->getCoreNumber() << endl;
		file << "-Total memory: " << process->getTotalMemory();

		file.close();

	}

	int getAvailableblocks() {
		int available_blocks = 0;

		for (int i = 0; i < memory.size(); i++) {
			if (memory.at(i) == 'F') {
				available_blocks++;
			}
		}

		return available_blocks;
	}

	bool checkAllocationMap(shared_ptr<Process> process) override {

		for (auto lt = allocationMap.begin(); lt != allocationMap.end(); lt++) {
			if (process->getPid() == lt->second->getPid()) {
				return true;
			}
		}

		return false;
	}

public:
	FlatMemoryAllocator(int max_overall_mem) {
		this->max_overall_mem = max_overall_mem;
		this->mem_per_block = 1;

		int nblocks = max_overall_mem / mem_per_block;

		for (int i = 0; i < nblocks; i++) {
			this->memory.push_back('F'); // Meaning memory is Free
		}
	}


	bool allocate(shared_ptr<Process> process) override {
		int required_memory = process->getTotalMemory();
		int required_blocks = required_memory / mem_per_block;

		if (checkAllocationMap(process)) {
			return true;
		}

		if (required_memory % mem_per_block != 0) {
			required_blocks++;
		}

		if (required_blocks > getAvailableblocks()) {
			backingStore();
		}

		for (int i = 0; i <= memory.size() - required_blocks; i++) {
			if (canAllocate(required_blocks, i)) {
				allocateAt(process, required_blocks, i);
				checkBackingStore(process);
				return true;
			}
		}

		return false;
	}

	void deallocate(shared_ptr<Process> process) override {
		int iblock = -1;
		int mapkey = -1;

		for (auto it = allocationMap.begin(); it != allocationMap.end(); it++) {
			if (it->second->getPid() == process->getPid()) {
				iblock = it->first / mem_per_block;
				mapkey = it->first;
				break;
			}
		}

		int blocks_consumed = process->getTotalMemory() / mem_per_block;
		if (process->getTotalMemory() % mem_per_block != 0) {
			blocks_consumed++;
		}

		for (int i = 0; i < blocks_consumed; i++) {
			memory.at(i + iblock) = 'F';
		}

		allocationMap.erase(mapkey);
	}

	int getTotalMemoryUsed() override {
		int used_blocks = 0;
		for (int i = 0; i < memory.size(); i++) {
			if (memory.at(i) == 'O') {
				used_blocks++;
			}
		}

		return used_blocks * mem_per_block;
	}

	map<int, shared_ptr<Process>> getAllocationMap() override {
		return this->allocationMap;
	}
};