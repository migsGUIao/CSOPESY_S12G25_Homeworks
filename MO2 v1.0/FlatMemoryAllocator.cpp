#include "MemoryAllocator.h"

class FlatMemoryAllocator : public MemoryAllocator {
	FlatMemoryAllocator(int max_overall_mem, int mem_per_frame) {
		this->max_overall_mem = max_overall_mem;
		/*this->mem_per_frame = mem_per_frame;*/
	}

};