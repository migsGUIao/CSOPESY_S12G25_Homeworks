#include <iostream>
#include <string.h>
#include <ctime>
#include <iomanip>
#include <unordered_map>
#include "Colors.h"

using namespace std;

struct ProcessInfo {
    string name;
    int id;
    int currentLine;
    int totalLine;
    string timestamp;
    bool active;
};

unordered_map<string, ProcessInfo> processes;

void clearScreen() {
    #ifdef _WIN32
        system("cls");  // For Windows
    #else
        system("clear");  // For Unix/Linux/MacOS
    #endif
}

string getCurrentTimestamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    // MM/DD/YYYY HH:MM:SS AM/PM
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%m/%d/%Y, %I:%M:%S %p", ltm);
    return string(timeStr);
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

void toggleProcess(const string& processName) {

    static int processCtr = 0;

    if (processes.find(processName) != processes.end()) {
        // If process exists, it will be active
        // info is printed
        if (processes[processName].active) {
            displayProcessInfo(processName);
        } 
    } else {
        cout << "Process " << processName << " does not exist. Use 'screen -s <name> to create it." << endl;
    }
}

void createProcess(const string& processName) {
    static int processCtr = 0;

    if (processes.find(processName) == processes.end()) {
        ProcessInfo newProcess;
        newProcess.currentLine = 10; 
        newProcess.totalLine = 100;  
        newProcess.id = ++processCtr; // increment ID
        newProcess.timestamp = getCurrentTimestamp();
        newProcess.active = true;
        
        processes[processName] = newProcess;

        displayProcessInfo(processName);
    } else {
        cout << "Process " << processName << " already exists." << endl;
    }
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
            toggleProcess(processName);
            
        } else if(input == "scheduler-test") {
            cout << "Scheduler-test command recognized. Doing something\n";
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