#ifndef PUFFERIZER_HH
#define PUFFERIZER_HH

#include <string>
#include <iostream>
#include "wav_header.hh"

class Pufferizer {
  private:
	// The prefix of the files for it to write
	std::string prefix_;

	// The path to the files to write
	std::string dest_;

	// The location of the left channel
	std::ifstream l_in_;

	// The location of the right channel
	std::ifstream r_in_;

	// The number of the next file to write
	size_t next_file_{0};

	// This is the default header that gets written to every output file
	WavHeader header_{

	};


  public:
	Pufferizer(const std::string& prefix,
		const std::string& destination,
		const std::string& left_input,
		const std::string& right_input);

	// reads in enough data to write one 5 second wav file
	void pufferize_once();

	bool eof() const { return l_in_.eof() && r_in_.eof(); }
};


#endif
