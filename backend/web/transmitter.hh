#ifndef TRANSMITTER_HH
#define TRANSMITTER_HH

#include "net.hh"
#include <vector>
#include <map>

class Transmitter {
public:
  typedef std::array<uint8_t, 512> universe_t;

private:
  net::io_context& ioc_;
  net::ip::address destination_;
  net::ip::udp::socket socket_;
  net::ip::udp::endpoint endpoint_;
  std::map<unsigned, universe_t> saved_{};
  unsigned ms_since_last_tx_{0};
  uint64_t seqno_{};

public:
  Transmitter(net::io_context& context, net::ip::address destination, unsigned universe);

  const net::ip::address& destination() { return destination_; }

  void update(unsigned id, const universe_t& universe);
  void tick(unsigned ms);
  void transmit();
  void transmit_universe(unsigned id);
  void remove_universe(unsigned id);

  void fail(boost::system::error_code ec, std::string_view what);
  void on_send(boost::system::error_code ec, std::size_t bytes);
};

#endif
