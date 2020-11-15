#include "wav_header.hh"

inline WavHeader::std::ostream& operator<<(std::ostream& os, WavHeader& header) {
	os.write(reinterpret_cast<char*>(&header), sizeof(header));
	return os;
}

WavHeader WavHeader::read_wav_header(std::ifstream& file) {
	WavHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	return header;
}
