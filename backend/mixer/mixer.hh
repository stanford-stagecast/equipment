#ifndef MIXER_HH
#define MIXER_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>

struct WavHeader {
	std::string riff{};			// Always 'RIFF'
	uint32_t file_size{};		// size of file
	std::string wave{};			// Always 'WAVE'
	std::string fmt{};			// Always 'fmt\0'
	uint32_t fmt_size{};		// size of preceeding data
	uint16_t type{};			// 1 for PCM and 2 for byte integer
	uint16_t channels{};		// number of channels
	uint32_t sample_rate{};		// sample rate. Usually 44100 Hz
	uint32_t data_rate{};		// average bytes per second
	uint16_t block_size{};		// size in bytes of each block of data
	uint16_t sample_size{};		// bits per sample
	std::string data{};			// Always 'data'
	uint32_t data_size{};		// number of bytes in the data
};

inline std::ostream& operator<<(std::ostream& os, WavHeader& header) {
	os << header.riff;
	os.write(reinterpret_cast<char*>(&header.file_size), sizeof(header.file_size));
	os << header.wave;
	os << header.fmt;
	os.write(reinterpret_cast<char*>(&header.fmt_size), sizeof(header.fmt_size));
	os.write(reinterpret_cast<char*>(&header.type), sizeof(header.type));
	os.write(reinterpret_cast<char*>(&header.channels), sizeof(header.channels));
	os.write(reinterpret_cast<char*>(&header.sample_rate), sizeof(header.sample_rate));
	os.write(reinterpret_cast<char*>(&header.data_rate), sizeof(header.data_rate));
	os.write(reinterpret_cast<char*>(&header.block_size), sizeof(header.block_size));
	os.write(reinterpret_cast<char*>(&header.sample_size), sizeof(header.sample_size));
	os << header.data;
	os.write(reinterpret_cast<char*>(&header.data_size), sizeof(header.data_size));

	return os;
}

class Mixer {
  private:
	std::ifstream file1;
	std::ifstream file2;

	WavHeader read_wav_header(std::ifstream& file);

  public:
	Mixer(const std::string name1, const std::string name2);
	~Mixer();
};

#endif
