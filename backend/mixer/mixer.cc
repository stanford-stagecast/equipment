#include "mixer.hh"
#include "wav_header.hh"

using namespace std;

// TODO: Make the constructor open the channels in a way that the audio server likes
// TODO:
Mixer::Mixer(vector<string> filenames, string l_output, string r_output)
  : l_out_{l_output}
  , r_out_{r_output} {
	if (filenames.size() == 0) {
		throw "Must supply at least one file to open";
	}

	if(!l_out_) {
		cerr << "failed to open left channel on " << l_output << endl;
	} else {
		cerr << "sucessfully opened left channel on " << l_output << endl;
	}

	if(!r_out_) {
		cerr << "failed to open right channel on " << r_output << endl;
	} else {
		cerr << "sucessfully opened right channel on " << r_output << endl;
	}

	for (string name : filenames) {
		streams_.push_back(ifstream{name, ifstream::binary});
		if (!streams_.back()) {
			cerr << "could not open " << name << endl;
		} else {
			cerr << "opened " << name << " sucessfully." << endl;
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
	WavHeader header{};
	header = header.read_wav_header(streams_[0]);
	// l_out_ << header;
	// r_out_ << header;

	for (size_t i = 1; i < streams_.size(); i++) {
		header = header.read_wav_header(streams_[i]);
	}
}

bool Mixer::first_file_ended() { return streams_[0].eof(); }

Mixer::~Mixer() {
	for (size_t i = 0; i < streams_.size(); i++) {
		streams_[i].close();
	}
}
