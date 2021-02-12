#include "captioner.hh"

using namespace std;
using high_resolution_clock = std::chrono::high_resolution_clock;

/**
 * The constructor takes in a stream key string obtained from Youtube
 * as well as a path to
 */
Captioner::Captioner(string stream_key, string caption_path)
 	: target_{ "/closedcaption?cid=" + stream_key + "&seq="} {

	// Attempt to open the file, thowing an exepction if it cannot be opened
	captions_.open(caption_path);
	if (!captions_) {
		throw runtime_error( "Could not open " + caption_path );
	}

	// Reads the first line into next_line_
	read_next_line();

	// Resolves the hostname and connects
	auto const results = resolver_.resolve(host_, "http");
	stream_.connect(results);
}

/**
 * This method finds the current time and returns it in a string
 * formatted in a way the YouTube expects
 */
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

	// Ideally this would have the time in milliseconds and not just add 000
	// But I couldn't figure out how to do that.
	return time + "000";
}

/**
 * This method reads the next line from captions_ into next_line_
 */
void Captioner::read_next_line() {
	string line;
	if (!captions_) {
		cerr << "End of captions reached";
	}
	getline(captions_, line);
	next_line_ = line;
}

/**
 * This method sends the caption in next_line_ to YouTube
 */
void Captioner::send_next_caption() {
	// Constructs the payload
	string line = next_line_;
	string time = utc_time();
	string payload = time + '\n' + line + "<br>\n";

	// Reads the next line into next_line_
	read_next_line();

	// Constructs and sends HTTP request
	http::request<http::string_body> req{http::verb::post, target_ + to_string(seqno_), 11};
    req.set(http::field::host, host_);
	req.body() = payload;
	req.prepare_payload();
	http::write(stream_, req);

	// Increments seqno
	seqno_++;

	// Receives HTTP response
	http::response<http::dynamic_body> res;
	beast::flat_buffer buffer;
	http::read(stream_, buffer, res);

	// Reports errors
	if (res.result_int() != 200) {
		cerr << "Request returned code " << res.result_int() << endl;
	}
}
