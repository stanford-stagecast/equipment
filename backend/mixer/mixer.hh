#ifndef MIXER_HH
#define MIXER_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#include "levels.hh"

// Ensures that floats are 32 bits long.
static_assert(sizeof(float) == 4);

class Mixer {
  private:
	std::vector<std::ifstream> streams_{};

	// right audio channel
	std::ofstream r_out_{"right.out"};

	// left audio channel
	std::ofstream l_out_{"left.out"};

  public:
	Mixer(std::vector<std::string> filenames);
	void mix(std::vector<Level> levels);
	~Mixer();
};

#endif
