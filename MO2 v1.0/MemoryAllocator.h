#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <queue>
#include "Process.h"
using namespace std;

class MemoryAllocator
{
protected:
	int max_overall_mem; 
	map<int, shared_ptr<Process>> allocationMap;

	virtual bool canAllocate(int, int) = 0;
	virtual void allocateAt(shared_ptr<Process>, int, int) = 0;
	virtual void checkBackingStore(shared_ptr<Process>) = 0;
	// virtual void backingStore() = 0;
	virtual bool checkAllocationMap(shared_ptr<Process>) = 0;

public:
	virtual bool allocate(shared_ptr<Process>) = 0;
	virtual void deallocate(shared_ptr<Process>) = 0;
	virtual int getTotalMemoryUsed() = 0;
	virtual map<int, shared_ptr<Process>> getAllocationMap() = 0;
};

