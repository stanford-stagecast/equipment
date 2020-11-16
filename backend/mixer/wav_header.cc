#include "wav_header.hh"

// Default constructor doesn't need to do anything
WavHeader::WavHeader() : header_{} {}

// This lets the user construct a WavHeader from a WavHeader_struct
WavHeader::WavHeader(WavHeader_struct header) : header_{header} {}

std::ostream& operator<<(std::ostream& os, WavHeader header) {
	os.write(reinterpret_cast<char*>(&(header.header_)), sizeof(WavHeader_struct));
	return os;
}

WavHeader WavHeader::read_wav_header(std::ifstream& file) {
	WavHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	return header;
}
