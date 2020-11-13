#include "mixer.hh"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
	if (argc > 1) {

		Mixer mixer{std::vector<std::string>{argv + 1, argv + argc}};
	} else {
		std::cerr << "must supply filename to mix" << std::endl;
		return 1;
	}
	return 0;
}
