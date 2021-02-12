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

class Captioner {
  private:
	std::size_t seqno_{100000};
	std::ifstream captions_{};
	std::string host_{"upload.youtube.com"};
	std::string target_;
	std::string next_line_{};

	net::io_context ioc_{};
	tcp::resolver resolver_{ioc_};
	beast::tcp_stream stream_{ioc_};

	std::string utc_time();
	void read_next_line();

  public:
	Captioner(std::string stream_key, std::string caption_path);
	void send_next_caption();
	bool eof() { return captions_.eof(); }
	std::string const next_line() { return next_line_; }
};

#endif
