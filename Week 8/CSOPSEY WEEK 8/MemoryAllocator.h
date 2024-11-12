#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;

class MemoryAllocator
{
private:
	int max_size;
	int size_per_frame;
	map<int, string> memoryMap;
	vector<char> memoryFrame;
	bool canAllocate(int, int);
	void allocateAt(int, int, string);

public:
	MemoryAllocator(int, int);
	bool allocate(int, string);
	void deallocate(int, string);
	void printMemory(string, int, int);
	void sample(int);
};

