#ifndef MIXER_HH
#define MIXER_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#include "levels.hh"
#include "wav_header.hh"

// Ensures that floats are 32 bits long.
static_assert(sizeof(float) == 4);

class Mixer {
  private:
	std::vector<std::ifstream> streams_{};

	// right audio channel
	std::ofstream r_out_{};

	// left audio channel
	std::ofstream l_out_{};

  public:
    // the constructor takes in a vector of filenames and opens an ifstream to
    // each file in the vector
	Mixer(std::vector<std::string> filenames);

    // The mix method takes in the a vector of levels and mixes all the inputs
    // down to a left and a right channel
	void mix(std::vector<Level> levels);

    // This method reads, prints and removes the wav headers of the files.
    // It is useful when reading from wav files
    void remove_wav_headers();
	~Mixer();
};

#endif
