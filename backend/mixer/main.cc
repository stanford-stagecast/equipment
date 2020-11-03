#include "mixer.hh"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc == 3) {
		Mixer mixer{argv[1], argv[2]};
	} else if (argc == 2) {
		Mixer mixer{argv[1], ""};
	} else {
		std::cerr << "incorrect number of arguments" << std::endl;
		return 1;
	}
	return 0;
}
