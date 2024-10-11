#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include "Colors.h"

using namespace std;

struct ProcessInfo {
    string name;
    int id;
    int currentLine;
    int totalLine;
    string timestamp;
    bool active;
    int coreNumber;
};

unordered_map<string, ProcessInfo> processes;
queue<string> processQueue;  
mutex mtx;                    
vector<string> finishedProcesses; 

void clearScreen() {
    #ifdef _WIN32
        system("cls");  // For Windows
    #else
        system("clear");  // For Unix/Linux/MacOS
    #endif
}

const int NUM_CORES = 4;     
int coreAvailability[NUM_CORES] = {0};  

string getCurrentTimestamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    // MM/DD/YYYY HH:MM:SS AM/PM
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%m/%d/%Y %I:%M:%S%p", ltm);
    return string(timeStr);
}

// Simulate a process running on a core
void runProcess(string processName) {
    // Get the process info
    ProcessInfo& process = processes[processName];

    // Write to file
    ofstream outfile(processName + "_log.txt");
    outfile << "Process name: " << processName << endl;
    outfile << "Logs:\n" << endl;
    
    for (int i = 1; i <= process.totalLine; ++i) {
        this_thread::sleep_for(chrono::milliseconds(100));  // Simulate work

        string currentTimeStamp = getCurrentTimestamp();

        // print to console
        mtx.lock();
        cout << processName << "  (" << currentTimeStamp << ")" << "  Core: " << process.coreNumber << "   " << i << "/" << process.totalLine << endl;;
        mtx.unlock();

        // Write to the file
        outfile << "(" << currentTimeStamp << ")" << "   Core: " << process.coreNumber << "   'Hello world from screen " << processName << "!'" << endl;

        // Update progress in memory
        mtx.lock();
        process.currentLine = i;
        mtx.unlock();
    }

    mtx.lock();
    process.active = false;
    coreAvailability[process.coreNumber] = 0;

    // Mark final status in the file
    outfile << "\nProcess finished at line " << process.totalLine << "/" << process.totalLine << endl;
    outfile << "Status: Finished" << endl;
    outfile.close();  // Close the file when process is done

    finishedProcesses.push_back(processName);  // Add to finished list
    mtx.unlock();
}

// FCFS scheduler that assigns processes to available cores
void fcfsScheduler() {
    while (!processQueue.empty()) {
        mtx.lock();
        string processName = processQueue.front();
        processQueue.pop();
        mtx.unlock();

        // Find an available core
        int assignedCore = -1;
        while (assignedCore == -1) {
            mtx.lock();
            for (int i = 0; i < NUM_CORES; i++) {
                if (coreAvailability[i] == 0) {  // Core is available
                    coreAvailability[i] = 1;     // Core is now currently in use
                    assignedCore = i;
                    break;
                }
            }
            mtx.unlock();
            if (assignedCore == -1) {
                this_thread::sleep_for(chrono::milliseconds(100));  // Wait for a core to be free
            }
        }

        // Assign process to the core
        mtx.lock();
        processes[processName].coreNumber = assignedCore;
        processes[processName].active = true;
        mtx.unlock();

        // Run the process in a new thread and store it
        thread t(runProcess, processName);
        t.detach();
    }
}

void displayProcessInfo(const string& processName) {
    // Check if process exists
    clearScreen();
    ProcessInfo &info = processes[processName];

    cout << "Process: " << processName << endl;
    cout << "ID: " << info.id << endl;
    cout << "\nCurrent instruction line: " << info.currentLine << endl;
    cout << "Lines of code: " << info.totalLine << endl;
    cout << "===============================" << endl;
    cout << "Created At: " << info.timestamp << endl;

    string command;
    while (true) {
        cout << "\nType 'exit' to return to the main menu!" << endl;
        cout << "root:\\> ";
        getline(cin, command);

        if (command == "exit") {
            break;  
        } else {
            cout << "\nInvalid command. Type 'exit' to go back.";
        }
    }
    
    clearScreen();
}

void createProcess(const string& processName) {
    static int processCtr = 0;

    if (processes.find(processName) == processes.end()) {
        ProcessInfo newProcess;
        newProcess.currentLine = 10; 
        newProcess.totalLine = 100; // 100 lines to print
        newProcess.id = ++processCtr; // increment ID
        newProcess.timestamp = getCurrentTimestamp();
        newProcess.active = true;
        newProcess.coreNumber = -1;
        
        processes[processName] = newProcess;

        mtx.lock();
        processQueue.push(processName);
        mtx.unlock();
    } else {
        cout << "Process " << processName << " already exists." << endl;
    }
}

void retrieveProcess(const string& processName) {

    static int processCtr = 0;

    if (processes.find(processName) != processes.end()) {
        displayProcessInfo(processName);
    } else {
        cout << "Process " << processName << " does not exist. Use 'screen -s <name> to create it." << endl;
    }
}

void schedulerTest() {
    string processName;

    // create 10 processes
    for (int i = 1; i <= 10; i++) {
        processName = "process0" + to_string(i);
        if (i > 9)
            processName = "process" + to_string(i);
        createProcess(processName);
    }

    cout << "------------------------------------------------" << endl;
    cout << "Running processes:" << endl;
    // Start the FCFS scheduler
    fcfsScheduler();

    this_thread::sleep_for(chrono::seconds(20));

    // Display finished process details
    cout << "\nFinished processes:\n";
    for (const string& p : finishedProcesses) {
        cout << p << "  (" << processes[p].timestamp << ")"  << "  Finished  " << processes[p].currentLine << "/" << processes[p].totalLine << endl;
    }
    cout << "------------------------------------------------" << endl;
}

int main() {
    string processName, input;

    while(input != "exit") {
        cout <<"   ____ ____   ___  ____  _____ ______   __         " << endl;
        cout <<"  / ___/ ___| / _ \\|  _ \\| ____/ ___\\ \\ / /     " << endl;
        cout <<" | |   \\___ \\| | | | |_) |  _| \\___ \\\\ v /     " << endl;
        cout <<" | |___ ___) | |_| |  __/| |___ ___) || |           " << endl;
        cout <<"  \\____|____/ \\___/|_|   |_____|____/ |_|         " << endl;
        cout << GRN;
        cout <<"Hello Welcome to CSOPESY commandline!\n";
        cout << reset;

        cout <<"COMMANDS\n";
        cout <<"_____________________________________________\n";
        cout <<"'initalize'\n";
        cout <<"'screen -s <name>'\n";
        cout <<"'screen -r <name>'\n";
        cout <<"'scheduler-test'\n";
        cout <<"'report-until'\n";
        cout <<"'clear'\n";
        cout <<"'exit'\n";
        cout <<"_____________________________________________\n";

        cout << YEL;
        cout << "Type 'exit' to quit, 'clear' to clear the screen\n";
        cout << reset;
        cout <<"Enter a Command: ";
        getline(cin, input);

        if(input == "initialize") {
            cout << "Initialize command recognized. Doing something\n";
        } else if (input.substr(0, 10) == "screen -s ") {
            processName = input.substr(10);
            createProcess(processName);
        } else if (input.substr(0, 10) == "screen -r ") {
            processName = input.substr(10);
            retrieveProcess(processName);
        } else if(input == "scheduler-test") {
            schedulerTest();
        } else if(input == "report-until") {
            cout << "Report-until command recognized. Doing something\n";
        } else if(input == "clear") {
            clearScreen();
        }  else if(input == "exit") {
            exit(0);
        } else{
            cout << "Invalid command!\n";
        }
    }

    return 0;
}