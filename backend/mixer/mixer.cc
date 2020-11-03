#include "mixer.hh"

using namespace std;

Mixer::Mixer(vector<string> filenames) {
	for (string name : filenames) {
		files_.push_back(ifstream{name, ifstream::binary});
		if (!files_.back()) {
			cerr << "could not open " << name << endl;
		} else {
			cout << "opened " << name << " sucessfully." << endl;
		}
	}

	ofstream of{"out.wav", ofstream::binary};
	if (!of) cerr << "could not create out.wav" << endl;

	WavHeader header = read_wav_header(files_[0]);
	of << header;
	for (size_t i = 1; i < files_.size(); i++) {
		read_wav_header(files_[i]);
	}

	char tmp[2];
	while (true) {
		int16_t sample = 0;
		for (size_t i = 0; i < files_.size(); i++) {
			files_[i].read(tmp, sizeof(tmp));
			sample += *reinterpret_cast<int16_t*>(&tmp) / files_.size();
		}
		of.write(reinterpret_cast<char*>(&sample), sizeof(tmp));
		if (files_[0].eof()) break;
	}
}

WavHeader Mixer::read_wav_header(ifstream& file) {
	WavHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	return header;
}

Mixer::~Mixer() {
	for (size_t i = 0; i < files_.size(); i++) {
		files_[i].close();
	}
}
