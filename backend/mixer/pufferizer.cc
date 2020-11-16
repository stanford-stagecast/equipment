#include "pufferizer.hh"

using namespace std;

Pufferizer::Pufferizer(const string& prefix,
					   const string& destination,
					   const string& left_input,
					   const string& right_input)
	: prefix_{prefix}
	, dest_{destination}
	, l_in_{left_input}
	, r_in_{right_input} {}

bool Pufferizer::eof() const {
	 return l_in_.eof() && r_in_.eof();
}

inline int16_t Pufferizer::float_to_sample( const float sample_f ) {
	constexpr float maxval = uint64_t( 1 ) << 15;
	return lrint( clamp( sample_f, -1.0f, 1.0f ) * maxval );
}

void Pufferizer::pufferize_once() {
	ofstream os{dest_ + "/" + prefix_ + to_string(next_file_)};
}
