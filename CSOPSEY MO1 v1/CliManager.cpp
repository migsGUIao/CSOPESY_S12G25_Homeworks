#include "CliManager.h"

CliManager::CliManager()
{
	string command;
	bool initialized = false;
	thread t_scheduler;

	while (true) {
		command = layout.mainScreen();
		vector<string> commandArgs = tokenizer(command);

		if (command == "initialize" && !initialized) {
			cout << CYN << command << " recognized. Getting values from Config.txt." << endl;
			scheduler = make_unique<Scheduler>("config.txt");
			cout << "Done!!" << endl;
			initialized = true;
		}
		else if (commandArgs[0] == "screen" && initialized) {
			screenCommand(commandArgs);
		}
		else if (command == "scheduler-test" && initialized) {
			if (!t_scheduler.joinable()) {
				cout << CYN << "Turning on scheduler. . ." << endl;
				t_scheduler = thread(&Scheduler::startSchedule, scheduler);
			}
			else {
				cout << MAG << "Error: Scheduler is already turned on." << endl;
			}
		}
		else if (command == "scheduler-stop" && initialized) {
			if (t_scheduler.joinable()) {
				scheduler->stopSchedule();
				t_scheduler.join();
				cout << CYN << "Scheduler has been stopped. . ." << endl;
			}
			else {
				cout << MAG << "Error: Scheduler has not been instantiated yet!!!" << endl;
			}
		}
		else if (command == "report-util" && initialized) {
			processes = scheduler->getProcessMap();
			if (layout.reportutil_Screen(processes, scheduler->getNumCpu(), scheduler->getStatus())) {
				cout << CYN << "Report generated at \"csopey-log.txt\"" << endl;
			}
			else {
				cout << MAG << "Error: Unable to open or create file" << endl;
			}
		}
		else if (command == "clear" && initialized) {
			layout.clearScreen();
		}
		else if (command == "exit") {
			cout << CYN << "Exting . . ." << COLOR_RESET << endl;
			break;
		}
		else {
			cout << MAG << "Error: " << command << " not recognized. Aborting. . ." << endl;
		}

	}
}

vector<string> CliManager::tokenizer(string input)
{
	stringstream strStream(input);
	string token;
	vector<string> tokens;

	while (strStream >> token) {
		tokens.push_back(token);
	}

	return tokens;
}

void CliManager::screenCommand(vector<string> commandArgs)
{
	if (commandArgs.size() == 1) {
		cout << MAG << "Invalid parameters\n" << endl << COLOR_RESET;
		return;
	}

	processes = scheduler->getProcessMap();

	if (commandArgs.size() == 3 && commandArgs[1] == "-r") {
		if (processes.find(commandArgs[2]) != processes.end()) {
			layout.clearScreen();
			layout.screen_r_Screen(processes[commandArgs[2]]);
			layout.clearScreen();
		}
		else {
			cout << MAG << "Process " << commandArgs[2] << " does not exist. Use 'screen -s <name> to create it.\n" << endl
				<< COLOR_RESET;
		}
	}
	else if (commandArgs.size() == 3 && commandArgs[1] == "-s") {
		if (processes.find(commandArgs[2]) != processes.end()) {
			cout << MAG << "Process " << commandArgs[2] << " has already been created.\n" << endl << COLOR_RESET;
		}
		else {
			scheduler->createProcess(commandArgs[2]);
			cout << CYN << commandArgs[2] << " has been created\n" << endl;
		}
	}
	else if (commandArgs.size() == 2 && commandArgs[1] == "-ls") {
		layout.screen_ls_Screen(processes, scheduler->getNumCpu(), scheduler->getStatus());
	}
	else {
		cout << MAG << "Error: Command not recognized. Aborting." << endl;
	}
}
