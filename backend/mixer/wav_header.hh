#ifndef WAV_HEADER_HH
#define WAV_HEADER_HH

#include <iostream>
#include <fstream>

struct __attribute__ ((packed)) WavHeader {
	char riff[4];			// Always 'RIFF'
	uint32_t file_size{};		// size of file
	char wave[4];			// Always 'WAVE'
	char fmt[4];			// Always 'fmt\0'
	uint32_t fmt_size{};		// size of preceeding data
	uint16_t type{};			// 1 for PCM and 2 for byte integer
	uint16_t channels{};		// number of channels
	uint32_t sample_rate{};		// sample rate. Usually 44100 Hz
	uint32_t data_rate{};		// average bytes per second
	uint16_t block_size{};		// size in bytes of each block of data
	uint16_t sample_size{};		// bits per sample
	char data[4];			// Always 'data'
	uint32_t data_size{};		// number of bytes in the data
};

std::ostream& operator<<(std::ostream& os, WavHeader& header);

WavHeader read_wav_header(std::ifstream& file);

#endif
