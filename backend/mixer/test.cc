#include "mixer.hh"
#include <iostream>
#include <vector>

using namespace std;

int main() {
	cout << "This test uses relative directories and only work when run in the build directory" << endl;
	Mixer mixer({"../tests/float_440_hz_tone.wav", "../tests/float_441_hz_tone.wav"}
				, "left.wav", "right.wav");
	mixer.remove_wav_headers();

	while (!mixer.first_file_ended()) {
		mixer.mix({{1, 1}, {-1, 1}});
	}

	return 0;
}
