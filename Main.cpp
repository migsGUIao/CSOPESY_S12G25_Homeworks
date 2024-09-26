#include <iostream>
#include <string.h>
#include <ctime>
#include <iomanip>
#include "Colors.c"

using namespace std;
string input;

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
    
    // Time format: mm/dd/yyyy hh:mm:ss AM/PM
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%m/%d/%Y, %I:%M:%S %p", ltm);
    return string(timeStr);
}

// Function to draw layout based on the '-r <name>' command
void displayProcessInfo(const string& processName) {
    int currentLine = 10; // Example current line of instruction
    int totalLine = 100;  // Example total lines of instruction
    int ID = 50; // Example of ID
    
    string timestamp = getCurrentTimestamp();

    clearScreen();
    
    cout << "Process: " << processName << endl;
    cout << "ID: " << ID << endl;
    cout << "\nCurrent instruction line: " << currentLine << endl;
    cout << "Lines of code: " << totalLine << endl;
    cout << "===============================" << endl;
    cout << "Created At: " << timestamp << endl;
    
    // Wait for user to type 'exit to return to the main menu
    string command;
    while (true) {
        cout << "\nType 'exit' to return to the main menu!" << endl;
        cout << "root:\\> ";
        getline(cin, command);

        if (command == "exit") {
            break;  // Exit the loop and return to the main menu
        } else {
            cout << "\nInvalid command. Type 'exit' to go back.";
        }
    }
    clearScreen();
}


int main() {
    string processName;

    while(input != "exit"){
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
        cout <<"'screen'\n";
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

        if(input == "initialize"){
            cout << "Initialize command recognized. Doing something\n";
        }else if(input == "screen"){
            cout << "Screen command recognized. Doing something\n";
        }else if(input == "scheduler-test"){
            cout << "Scheduler-test command recognized. Doing something\n";
        }else if(input == "report-until"){
            cout << "Report-until command recognized. Doing something\n";
        }else if (input.substr(0, 10) == "screen -r ") {
            processName = input.substr(10);
            displayProcessInfo(processName);
        }else if(input == "clear"){
            clearScreen();
        }else if(input == "exit"){
            exit(0);
        }else{
            cout << "Invalid command!\n";
        }
    }

    return 0;
}