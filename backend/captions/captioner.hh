#ifndef CAPTIONER_HH
#define CAPTIONER_HH

#include <string>		// for string
#include <iostream>		// for cout and cerr
#include <fstream>		// for ifstream
#include <stdexcept>	// for runtime_error
#include <chrono>		// for time stuff

// For HTTP functionality
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

/**
 * A class for sending closed captions to YouTube livestreams
 */
class Captioner {
  private:
	// Youtube requires each caption to be sent with a sequence number
	std::size_t seqno_{1};

	// This stream points to the file from which captions are read
	std::ifstream captions_{};

	// Captions only go to YouTube
	std::string host_{"upload.youtube.com"};

	// This string fills in everything else in the needed url
	std::string target_;

	// This string stores the next line that the captioner will send
	std::string next_line_{};

	// This is all stuff that Boost needs to run the HTTP connection
	net::io_context ioc_{};
	tcp::resolver resolver_{ioc_};
	beast::tcp_stream stream_{ioc_};

	// This helper function returns the current time in
	// a format that works for YouTube
	std::string utc_time();

	// This helper function reads the next line from captions_ into next_line_
	void read_next_line();

  public:
	// The constructor takes in a stream key and
	// a path to the file containting the captions.
	// Captions are read in one line at a time.
	Captioner(std::string stream_key, std::string caption_path);

	// This method sends the caption stored in next_line_ to YouTube
	void send_next_caption();

	// True if captions_ has reached eof. False otherwise
	bool eof() { return captions_.eof(); }

	// This function lets the caller peek at the next caption to send
	// without modifying anything
	std::string const next_line() { return next_line_; }
};

#endif
