#include <iostream>
#include <string.h>
#include "Colors.c"

using namespace std;
string input;

int main() {
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
    cin >> input;

    if(input == "initialize"){
        cout <<"Initialize command recognized. Doing something\n";
    }else if(input == "screen"){
        cout <<"Screen command recognized. Doing something\n";
    }else if(input == "scheduler-test"){
        cout <<"Scheduler-test command recognized. Doing something\n";
    }else if(input == "report-until"){
        cout <<"Report-until command recognized. Doing something\n";
    }else if(input == "clear"){
        //system("cls");
        system("clear");
    }else if(input == "exit"){
        exit(0);
    }
    else{
        cout <<"Invalid command!\n";
    }

    }

    return 0;
}