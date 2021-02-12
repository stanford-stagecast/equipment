#include "captioner.hh"

using namespace std;
using high_resolution_clock = std::chrono::high_resolution_clock;

Captioner::Captioner(string stream_key, string caption_path)
 	: target_{ "/closedcaption?cid=" + stream_key + "&seq="} {

	// Attempt to open the file, thowing an exepction if it cannot be opened
	captions_.open(caption_path);
	if (!captions_) {
		throw runtime_error( "Could not open " + caption_path );
	}

	auto const results = resolver_.resolve(host_, "http");
	stream_.connect(results);
	read_next_line();
}

void Captioner::read_next_line() {
	string line;
	if (!captions_) {
		cerr << "End of captions reached";
	}
	getline(captions_, line);
	next_line_ = line;
}

void Captioner::send_next_caption() {
	string line = next_line_;
	string time = utc_time();
	string payload = time + '\n' + line + '\n';

	read_next_line();

	cerr << payload;

	http::request<http::string_body> req{http::verb::post, target_ + to_string(seqno_), 11};
	seqno_++;
    req.set(http::field::host, host_);

	req.body() = payload;
	req.prepare_payload();

	// req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	http::write(stream_, req);

	http::response<http::dynamic_body> res;

	beast::flat_buffer buffer;
	// Receive the HTTP response
	http::read(stream_, buffer, res);

	if (res.result_int() != 200) {
		cerr << "Request returned code " << res.result_int() << endl;
	}
}

string Captioner::utc_time() {
	high_resolution_clock::time_point now = high_resolution_clock::now();

	time_t tt = high_resolution_clock::to_time_t( now );
	struct tm* utc = gmtime(&tt);
	char c_time[100];

	size_t len = strftime(c_time, sizeof(c_time), "%Y-%m-%dT%H:%M:%S.", utc);

	if (len == 0) {
		throw runtime_error( "strftime returned 0" );
	}

	string time(c_time, c_time + len);
	return time + "000";
}
