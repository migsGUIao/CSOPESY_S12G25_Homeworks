#include "MemoryAllocator.h"

bool MemoryAllocator::canAllocate(int index, int frames_required)
{
	for (int i = 0; i < frames_required; i++) {
		if (memoryFrame.at(i + index) == 'O') { // O means Occupied
			return false;
		}
	}

	return true;
}

void MemoryAllocator::allocateAt(int index, int frames_required, string name)
{
	for (int i = 0; i < frames_required; i++) {
		memoryFrame.at(i + index) = 'O';
	}

	memoryMap[index * size_per_frame] = name;
}

MemoryAllocator::MemoryAllocator(int memSize, int mem_per_frame)
{
	this->max_size = memSize;
	this->size_per_frame = mem_per_frame;
	int nFrames = memSize / mem_per_frame;

	for (int i = 0; i < nFrames; i++) {
		this->memoryFrame.push_back('F'); // F means Free
	}
}

bool MemoryAllocator::allocate(int size, string name)
{
	int frames_required = size / size_per_frame;

	if (frames_required % size_per_frame != 0) {
		frames_required++;
	}

	for (int i = 0; i <= memoryFrame.size() - frames_required; i++) {
		if (canAllocate(i, frames_required)) {
			allocateAt(i, frames_required, name);
			return true;
		}
	}

	return false;
}

void MemoryAllocator::deallocate(int size, string name)
{
	int iframe = -1;
	int key = -1;

	for (auto it = memoryMap.begin(); it != memoryMap.end(); it++) {
		if (it->second == name) {
			iframe = it->first / size_per_frame;
			key = it->first;
			break;
		}
	}

	int frames_required = size / size_per_frame;
	if (frames_required % size_per_frame != 0) {
		frames_required++;
	}

	for (int i = 0; i < frames_required; i++) {
		memoryFrame.at(i + iframe) = 'F';
	}

	memoryMap.erase(key);
}

void MemoryAllocator::printMemory(string time, int qq, int mem_per_proc)
{
	string subfolder = "memory_files";

	if (!filesystem::exists(subfolder)) {
		filesystem::create_directory(subfolder);
	}

	ofstream file(subfolder + "/memory_stamp_" + to_string(qq) + ".txt");
	file << "Timestamp: " << time << endl;
	file << "Number of processes in the memory: " << memoryMap.size() << endl;

	int externalFragmentation = 0;

	for (int i = 0; i < memoryFrame.size(); i++) {
		if (memoryFrame.at(i) == 'O') {
			externalFragmentation++;
		}
	}
	externalFragmentation *= size_per_frame;
	externalFragmentation = max_size - externalFragmentation;

	file << "Total external fragmentation in KB: " << externalFragmentation << endl << endl;
	file << "----end---- = " << max_size << endl << endl;

	int upperBound = -1;
	int lowerBound = -1;
	string processName = "";
	for (auto it = memoryMap.rbegin(); it != memoryMap.rend(); it++) {

		lowerBound = it->first;
		processName = it->second;
		upperBound = lowerBound + mem_per_proc;

		file << upperBound << endl;
		file << processName << endl;
		file << lowerBound << endl << endl;
	}

	file << "----start---- = " << 0;
	file.close();
}

void MemoryAllocator::sample(int qq)
{
	ofstream file("memory_sample.txt");
	file << "HELLO " << qq;
	file.close();
}


