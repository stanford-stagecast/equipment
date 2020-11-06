#ifndef MIXER_HH
#define MIXER_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>

#include "wav_header.hh"

inline std::ostream& operator<<(std::ostream& os, WavHeader& header) {
	os.write(reinterpret_cast<char*>(&header), sizeof(header));
	return os;
}

class Mixer {
  private:
	std::vector<std::ifstream> files_{};

	WavHeader read_wav_header(std::ifstream& file);

  public:
	Mixer(std::vector<std::string> filenames);
	~Mixer();
};

#endif
