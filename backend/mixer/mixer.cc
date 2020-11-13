#include "mixer.hh"

using namespace std;

// TODO: Make the constructor open the channels in a way that the audio server likes
// TODO:
Mixer::Mixer(vector<string> filenames) {
	for (string name : filenames) {
		streams_.push_back(ifstream{name, ifstream::binary});
		if (!streams_.back()) {
			cerr << "could not open " << name << endl;
		} else {
			cout << "opened " << name << " sucessfully." << endl;
		}
	}
}

void Mixer::mix(vector<Level> levels) {
	char tmp[sizeof(float)];
	float right_sample = 0;
	float left_sample = 0;
	for (size_t i = 0; i < streams_.size(); i++) {
		streams_[i].read(tmp, sizeof(tmp));
		right_sample += *reinterpret_cast<float*>(&tmp) * levels[i].level * ((levels[i].pan / 2) + 0.5);
		left_sample += *reinterpret_cast<float*>(&tmp) * levels[i].level * (-(levels[i].pan / 2) + 0.5);
	}

	r_out_.write(reinterpret_cast<char*>(&right_sample), sizeof(tmp));
	l_out_.write(reinterpret_cast<char*>(&left_sample), sizeof(tmp));
}

void Mixer::remove_wav_headers() {
	for (size_t i = 0; i < streams_.size(); i++) {
		WavHeader header;
		header.read_from_file(streams_[i]);
		cout << header << endl;
	}
}

Mixer::~Mixer() {
	for (size_t i = 0; i < streams_.size(); i++) {
		streams_[i].close();
	}
}
