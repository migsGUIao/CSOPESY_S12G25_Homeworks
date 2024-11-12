#include "CliManager.h"

void cleanUp(string folder_path) {

	if (!filesystem::exists(folder_path)) {
		filesystem::create_directory(folder_path);
		return;
	}

	if (distance(filesystem::directory_iterator(folder_path), filesystem::directory_iterator{}) != 0) {
		for (const auto& entry : filesystem::directory_iterator(folder_path)) {
			if (filesystem::is_regular_file(entry.status())) {
				filesystem::remove(entry.path());
			}
		}
	}
}

int main(int argc, char** argv) {

	cleanUp("memory_files");
	CliManager climanager;

	return 0;
}

