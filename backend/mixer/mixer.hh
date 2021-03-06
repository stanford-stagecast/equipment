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

	// left audio channel
	std::ofstream l_out_;

	// right audio channel
	std::ofstream r_out_;

  public:
    // the constructor takes in a vector of filenames and opens an ifstream to
    // each file in the vector
	Mixer(std::vector<std::string> filenames, std::string l_output, std::string r_output);

    // The mix method takes in the a vector of levels and mixes all the inputs
    // down to a left and a right channel
	void mix(std::vector<Level> levels);

    // This method reads, prints and removes the wav headers of the files. And
    // then writes the wav header to the left and right outputs. It's only
    // useful when mixing wav files
    void remove_wav_headers();

    // returns true if the EOF bit is set on streams_[0]
    bool first_file_ended();

    // The destructor loops over the open file descriptors and closes them
	~Mixer();
};

#endif
