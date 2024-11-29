CSOPESY - MCO2 Members: Andreo Canilao, Bryle Magura, Migs Guiao, Ralph Garcia

Header files: [Contains function prototypes and fields of the corresponding class (except Colors.h which contains macros to color the text in the command line)]

Colors.h
Scheduler.h
CliLayout.h
Process.h
CliManager.h
MemoryAllocator.h
C++ files:

Main.cpp - Contains the main function
Scheduler.cpp - Contains the scheduler algorithms, program parameters, and the list of all processes.
CliLayout.cpp - Contains the layout used for program output.
Process.cpp - Contains process information.
CliManager.cpp - Manages the whole program and serves as an intermediary between all classes.
PagingAllocator.cpp - Contains the implementation for the paging allocator and is a child class of MemoryAllocator.
FlatMemoryAllocator.cpp - Contains the implementation for the flat memory allocator and is a child class of MemoryAllocator.
Misc:

config.txt - Contains program configurations.
csopsey-log.txt - Contains output from the command "report-util".
backing_store - A directory containing the backing store of processes.
How to run:

Download the Zip
Open "CSOPSEY MO2 v1.0" folder
Open "MO2 v1.0.sln" project
Configure the solution to run in C++ 20
Run the program by clicking the play button.
Enter the command "initialize" to recognize other commands
Type exit to terminate the OS emulator
