#include "CliLayout.h"

void CliLayout::clearScreen()
{
#if defined(_WIN32) || defined(_WIN64)
	system("cls");  // Windows
#else
	system("clear"); // Unix/Linux/Mac
#endif
}

string CliLayout::mainScreen()
{
	string command;

	cout << COLOR_RESET << "   _____  _____  ____  _____   _____ ________     __" << endl <<
		"  / ____|/ ____|/ __ \\|  __ \\ / ____|  ____\\ \\   / /" << endl <<
		" | |    | (___ | |  | | |__) | (___ | |__   \\ \\_/ /" << endl <<
		" | |     \\___ \\| |  | |  ___/ \\___ \\|  __|   \\   /" << endl <<
		" | |____ ____) | |__| | |     ____) | |____   | |" << endl <<
		"  \\_____|_____/ \\____/|_|    |_____/|______|  |_|" << endl;
	cout << GRN << "Hello, Welcome to the CSOPSEY command line!" << endl;
	cout << YEL << "Type 'exit' to quit, 'clear' to clear the screen" << endl;
	cout << COLOR_RESET << "Enter a command: ";
	getline(cin, command);

	return command;
}

void CliLayout::screen_r_Screen(shared_ptr<Process> process)
{
	string command;
	cout << "Process: " << process->getName() << endl;
	cout << "ID: " << process->getPid() << endl << endl;
	cout << "Current instruction line: " << process->getCurrLine() << endl;
	cout << "Lines of code: " << process->getTotalLine() << endl;
	cout << "===============================" << endl;
	cout << "Created At: " << process->getTimeCreated() << endl << endl;
	cout << "Type 'exit' to return to the main menu!" << endl;

	while (true) {
		cout << "root:\\> ";
		getline(cin, command);

		if (command == "exit") {
			break;
		}
		else if (command == "process-smi") {
			if (process->getCurrLine() != process->getTotalLine()) {
				cout << "Process: " << process->getName() << endl;
				cout << "ID: " << process->getPid() << endl << endl;

				cout << "Current instruction line: " << process->getCurrLine() << endl;
				cout << "Lines of code: " << process->getTotalLine() << endl << endl;
			}
			else {
				cout << "Process: " << process->getName() << endl;
				cout << "ID: " << process->getPid() << endl << endl;

				cout << "Finished!!" << endl << endl;
			}
		}
		else {
			cout << "BASH: " << command << endl << endl;
		}
	}
}

void CliLayout::screen_ls_Screen(map<string, shared_ptr<Process>> processes, int totalCpu, int activeCpu)
{
	int cores_used = activeCpu;
	int cores_free = totalCpu - cores_used;
	float cpu_util = static_cast<float>(cores_used * 100 / totalCpu);

	cout << endl << endl;
	cout << "CPU utilization: " << cpu_util << "%" << endl;
	cout << "Cores used: " << cores_used << endl;
	cout << "Cores available: " << cores_free << endl << endl;
	cout << string(100, '-') << endl;
	cout << "Running Processes: " << endl;
	for (auto lt = processes.begin(); lt != processes.end(); lt++) {
		if (lt->second->getCurrLine() < lt->second->getTotalLine()) {
			cout << left << setw(25) << lt->second->getName() << setw(35) << lt->second->getTimeCreated()
				<< setw(15) << "Core: " + to_string(lt->second->getCoreNumber()) << setw(10)
				<< to_string(lt->second->getCurrLine()) + "/" + to_string(lt->second->getTotalLine()) << endl;
		}
	}
	cout << endl;
	cout << "Finished Processes: " << endl;
	for (auto lt = processes.begin(); lt != processes.end(); lt++) {
		if (lt->second->getCurrLine() == lt->second->getTotalLine()) {
			cout << left << setw(25) << lt->second->getName() << setw(35) << lt->second->getTimeCreated()
				<< setw(15) << "Finished" << setw(10)
				<< to_string(lt->second->getCurrLine()) + "/" + to_string(lt->second->getTotalLine()) << endl;
		}
	}
	cout << string(100, '-') << endl;
}

bool CliLayout::reportutil_Screen(map<string, shared_ptr<Process>> processes, int totalCpu, int activeCpu)
{
	ofstream file("csopsey-log.txt");
	/*ofstream outFile("example.txt", ios::app);*/

	if (!file) {
		return false;
	}

	int cores_used = activeCpu;
	int cores_free = totalCpu - cores_used;
	float cpu_util = static_cast<float>(cores_used * 100 / totalCpu);

	file << "CPU utilization: " << cpu_util << "%" << endl;
	file << "Cores used: " << cores_used << endl;
	file << "Cores available: " << cores_free << endl << endl;
	file << string(100, '-') << endl;
	file << "Running Processes: " << endl;
	for (auto lt = processes.begin(); lt != processes.end(); lt++) {
		if (lt->second->getCurrLine() < lt->second->getTotalLine()) {
			file << left << setw(25) << lt->second->getName() << setw(35) << lt->second->getTimeCreated()
				<< setw(15) << "Core: " + to_string(lt->second->getCoreNumber()) << setw(10)
				<< to_string(lt->second->getCurrLine()) + "/" + to_string(lt->second->getTotalLine()) << endl;
		}
	}
	file << endl;
	file << "Finished Processes: " << endl;
	for (auto lt = processes.begin(); lt != processes.end(); lt++) {
		if (lt->second->getCurrLine() == lt->second->getTotalLine()) {
			file << left << setw(25) << lt->second->getName() << setw(35) << lt->second->getTimeCreated()
				<< setw(15) << "Finished" << setw(10)
				<< to_string(lt->second->getCurrLine()) + "/" + to_string(lt->second->getTotalLine()) << endl;
		}
	}
	file << string(100, '-') << endl;

	file.close();

	return true;
}

void CliLayout::process_smi_Screen(vector<string> info)
{
	cout << string(50, '-') << endl;
	cout << left << setw(5) << "|";
	cout << internal << setw(40) << "PROCESS-SMI V01.00 Driver Version: 01.00";
	cout << right << setw(5) << "|" << endl;
	cout << string(50, '-') << endl;
	cout << "CPU-Util: " << info.at(0) << "%" << endl;
	cout << "Memory Usage: " << info.at(1) << "KB  /  " << info.at(2) << "KB" << endl;
	cout << "MemoryUtil: " << info.at(3) << "%" << endl << endl;
	cout << string(50, '=') << endl;
	cout << "Running processes and memory usage: " << endl;
	cout << string(50, '-') << endl;

	if (info.size() == 4) {
		cout << "No processes currently running at the moment. . ." << endl;
	}
	else {
		for (int i = 0; i < info.size() - 4; i++) {
			if (i % 2 == 0) {
				cout << info.at(i + 4) << " ";
			}
			else {
				cout << info.at(i + 4) << "KB" << endl;
			}
		}
	}

	cout << string(50, '-') << endl;
}

void CliLayout::vmstatScreen(vector<string> info)
{
	cout << CYN << right << setw(10) << info.at(0);
	cout << left << setw(25) << " Total Memory" << endl;

	cout << right << setw(10) << info.at(1);
	cout << left << setw(25) << " Used Memory" << endl;

	cout << right << setw(10) << info.at(2);
	cout << left << setw(25) << " Free Memory" << endl;

	cout << right << setw(10) << info.at(3);
	cout << left << setw(25) << " Idle Cpu Ticks" << endl;

	cout << right << setw(10) << info.at(4);
	cout << left << setw(25) << " Active Cpu Ticks" << endl;

	cout << right << setw(10) << info.at(5);
	cout << left << setw(25) << " Total Cpu Ticks" << endl;

	cout << right << setw(10) << info.at(6);
	cout << left << setw(25) << " Num Paged In" << endl;

	cout << right << setw(10) << info.at(7);
	cout << left << setw(25) << " Num Paged Out" << endl;


	cout << right << setw(10) << info.at(8);
	cout << left << setw(25) << " Num Finished Processes" << endl;

	cout << COLOR_RESET;
}