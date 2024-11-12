For week 8 homework of CSOPSEY and continuation to MO1:
- Added MemoryAllocator class to represent processes running in the memory
- Files regarding the memory repsentation is located at the "memory_files" directory
- For convience, everytime the program starts, all of the contents of "memory_files" is deleted. Make sure to save before running the program.

Notes:
- Unsure regarding how the round robin w/first fit memory allocator works, for the program implementation. I assumed that once a process has finished its time slice, it is removed from the memory regardless if all of its lines of code are executed or not.
- Will need to fix some logic handling in some methods of MemoryAllocator and how it is implemented in the Scheduler class. This program is only designed to handle the test case provided in the canvas. It is basically INCOMPLETE.
