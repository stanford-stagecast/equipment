#include "mixer.hh"

using namespace std;

Mixer::Mixer(const string name1, const string name2)
	: file1{name1, ifstream::binary}
	, file2{name2, ifstream::binary} {
	if (!file1) cerr << "could not open " << name1 << endl;
	if (!file2) cerr << "could not open " << name2 << endl;

	ofstream of{"out.wav", ofstream::binary};
	if (!of) cerr << "could not create out.wav" << endl;

	WavHeader header1 = read_wav_header(file1);
	read_wav_header(file2);
	of << header1;

	uint16_t sample1;
	uint16_t sample2;
	char tmp[2];
	while (file1.read(tmp, sizeof(tmp))) {
		sample1 = *reinterpret_cast<uint16_t*>(&tmp);
		file2.read(tmp, sizeof(tmp));
		sample2 = *reinterpret_cast<uint16_t*>(&tmp);
		sample1 += sample2;
		of.write(reinterpret_cast<char*>(&sample1), sizeof(tmp));
	}
}

WavHeader Mixer::read_wav_header(ifstream& file) {
	WavHeader header;
	char data[sizeof(header)];
	file.read(data, sizeof(header));

	header.riff = string(data, 4);
	header.file_size = *reinterpret_cast<uint32_t*>(&data[4]);
	header.wave = string(data + 8, 4);
	header.fmt = string(data + 12, 4);
	header.fmt_size = *reinterpret_cast<uint32_t*>(&data[16]);
	header.type = *reinterpret_cast<uint16_t*>(&data[20]);
	header.channels = *reinterpret_cast<uint16_t*>(&data[22]);
	header.sample_rate = *reinterpret_cast<uint32_t*>(&data[24]);
	header.data_rate = *reinterpret_cast<uint32_t*>(&data[28]);
	header.block_size = *reinterpret_cast<uint16_t*>(&data[32]);
	header.sample_size = *reinterpret_cast<uint16_t*>(&data[34]);
	header.data = string(data + 36, 4);
	header.data_size = *reinterpret_cast<uint32_t*>(&data[40]);

	return header;
}

Mixer::~Mixer() {
	file1.close();
	file2.close();
}
