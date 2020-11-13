#include "mixer.hh"
#include <iostream>
#include <vector>

using namespace std;

int main() {
	Mixer mixer{{"../../tests/float_440_hz_tone.wav", "../../tests/float_441_hz_tone.wav"}};
	return 0;
}
