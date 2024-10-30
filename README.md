Rewrote the whole code in OOP paradigm to accomodate readability and scalability of code. The code contains 5 header and cpp files.

Header files: [Contains function prototypes and fields of the corresponding class (except Colors.h which contains macros to color the text in the command line)]
1) Colors.h
2) Scheduler.h
3) CliLayout.h
4) Process.h
5) CliManager.h

C++ files:
1) Main.cpp - Contains the main function
2) Scheduler.cpp - Contains the scheduler algorithms, program parameters, and the list of all processes.
3) CliLayout.cpp - Contains the layout used for program output.
4) Process.cpp - Contains process information.
5) CliManager.cpp - Manages the whole program and serves as an intermediary between all classes.

Misc:
1) config.txt - Contains program configurations.
2) csopsey-log.txt - Contains output from the command "report-util".

Note:
1) csopsey-log.txt doesn't use append mode. Meaning its contents get overwritten everytime the "report-util" command is called.
2) Run the program in C++ 20.
3) Use the latest version of the program.
4) The program does not check if the values for "config.txt" are correct as it is not required as said by Sir. Mantua.
