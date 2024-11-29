#include "MemoryAllocator.h"

class FlatMemoryAllocator : public MemoryAllocator {

private:
	int mem_per_frame;
	vector<char> memory;

	bool canAllocate(int required_frames, int index) override {
		for (int i = 0; i < required_frames; i++) {
			if (memory.at(i + index) == 'O') { // Meaning memory is occupied
				return false;
			}
		}

		return true;
	}

	void allocateAt(shared_ptr<Process> process, int required_frames, int index) override {
		// Allocate memory to the process and change frame status to 'O'
		for (int i = 0; i < required_frames; i++) {
			memory.at(i + index) = 'O'; // Meaning memory is occupied
		}

		// Record the memory to the allocationMap wherein key is equal to the address where 
		// the memoryis stored
		allocationMap[index * mem_per_frame] = process;
	}

	void checkBackingStore(shared_ptr<Process> process) override {
		string filename = "backing_store/" + process->getName() + ".txt";

		// Delete the backing store of the process if it exists
		if (filesystem::exists(filename)) {
			filesystem::remove(filename);
			//num_paged_in++;
		}
	}

	void backingStore(shared_ptr<Process> process) {

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

	int getAvailableFrames() {
		int available_frames = 0;

		for (int i = 0; i < memory.size(); i++) {
			if (memory.at(i) == 'F') {
				available_frames++;
			}
		}

		return available_frames;
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
	FlatMemoryAllocator(int max_overall_mem, int mem_per_frame) {
		this->max_overall_mem = max_overall_mem;
		this->mem_per_frame = mem_per_frame;
		
		int nFrames = max_overall_mem / mem_per_frame;

		for (int i = 0; i < nFrames; i++) {
			this->memory.push_back('F');
		}
	}

	bool allocate(shared_ptr<Process> process) override {
		int required_memory = process->getTotalMemory();
		int required_frames = required_memory / mem_per_frame;

		if (checkAllocationMap(process)) {
			return true;
		}

		if (required_memory % mem_per_frame != 0) {
			required_frames++;
		}
		
		/*
		if (required_frames > getAvailableFrames()) {
			backingStore();
		}
		*/
		// If not enough frames are available, evict the oldest process until there's space
		
		int required_frames_copy = required_frames;
		while (required_frames_copy > getAvailableFrames()) {
			if (!allocationMap.empty()) {
				// Evict the oldest process 
				auto oldest = allocationMap.begin();
				shared_ptr<Process> oldestProcess = oldest->second;
				deallocate(oldestProcess);
				backingStore(oldestProcess); // Store the evicted process in the backing store
			} 
			required_frames_copy--;
   	 	}

		for (int i = 0; i <= memory.size() - required_frames; i++) {
			if (canAllocate(required_frames, i)) {
				allocateAt(process, required_frames, i);
				checkBackingStore(process);
				return true;
			}
		}

		return false;
	}

	void deallocate(shared_ptr<Process> process) override {
		int iframe = -1;
		int mapkey = -1;

		for (auto it = allocationMap.begin(); it != allocationMap.end(); it++) {
			//if (it->second == process) {
			//	iframe = it->first / mem_per_frame;
			//	mapkey = it->first;
			//	break;
			//}
			if (it->second->getPid() == process->getPid()) {
				iframe = it->first / mem_per_frame;
				mapkey = it->first;
				break;
			}
		}

		int frames_consumed = process->getTotalMemory() / mem_per_frame;
		if (process->getTotalMemory() % mem_per_frame != 0) {
			frames_consumed++;
		}

		for (int i = 0; i < frames_consumed; i++) {
			memory.at(i + iframe) = 'F';
		}
		
		allocationMap.erase(mapkey);
	}

	int getTotalMemoryUsed() override {
		int used_frames = 0;
		for (int i = 0; i < memory.size(); i++) {
			if (memory.at(i) == 'O') {
				used_frames++;
			}
		}

		return used_frames * mem_per_frame;
	}

	map<int, shared_ptr<Process>> getAllocationMap() override {
		return this->allocationMap;
	}

};