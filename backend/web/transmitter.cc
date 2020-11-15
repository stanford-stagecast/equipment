#include "transmitter.hh"

#include <iostream>
#include <boost/core/ignore_unused.hpp>
#include <boost/bind.hpp>

#define RETX_TIME_MS 1000
#define OUTPUT_PORT 0x3334

Transmitter::Transmitter(net::io_context &context, net::ip::address destination,
                         unsigned universe)
    : ioc_(context), destination_(destination), socket_(context),
      endpoint_(destination, OUTPUT_PORT), universe_(universe) {
  if (destination.is_v4()) {
    std::cout << "Sending Universe " << universe << " levels to <"
              << destination << ">" << std::endl;
    socket_.open(net::ip::udp::v4());
  } else if (destination.is_v6()) {
    std::cout << "Sending Universe " << universe << " levels to <"
              << destination << ">" << std::endl;
    socket_.open(net::ip::udp::v6());
  } else {
    std::cerr << "Cannot send Universe " << universe << " levels to <"
              << destination << ">: invalid IP." << std::endl;
  }
}

void Transmitter::update(const universe_t &universe) {
  saved_ = universe;
  transmit();
}

void Transmitter::tick(unsigned ms) {
  ms_since_last_tx_ += ms;
  if (ms_since_last_tx_ >= RETX_TIME_MS) {
    transmit();
  }
}

struct __attribute__ ((packed)) dmx_msg {
  char magic[10]; // "Stagecast"
  uint64_t seqno;
  uint16_t universe;
  uint8_t data[512];
};
static_assert(sizeof(dmx_msg) == 532);

std::array<uint8_t, sizeof(dmx_msg)> serialize_packet(unsigned universe,
                                          Transmitter::universe_t levels,
                                          uint64_t seqno) {
  dmx_msg msg;
  memcpy(&msg.magic, "Stagecast", 10);
  msg.seqno = htons(seqno);
  msg.universe = htons(universe);
  memcpy(&msg.data, levels.data(), 512);

  std::array<uint8_t, sizeof(dmx_msg)> array;
  uint8_t *ptr = reinterpret_cast<uint8_t *>(&msg);
  for (size_t i = 0; i < sizeof(dmx_msg); i++) {
    array[i] = ptr[i];
  }
  return array;
}

void Transmitter::transmit() {
  ms_since_last_tx_ = 0;
  auto buffer = net::buffer(serialize_packet(universe_, saved_, seqno_));

  socket_.async_send_to(buffer, endpoint_, boost::bind(&Transmitter::on_send, this, net::placeholders::error, net::placeholders::bytes_transferred));

  seqno_++; // increment (and/or overflow)
}

void Transmitter::fail(boost::system::error_code ec, std::string_view what) {
  std::cerr << "Transmitter -- Error in `" << what << "`: " << ec.message()
            << std::endl;
}

void Transmitter::on_send(boost::system::error_code ec, std::size_t bytes) {
  boost::ignore_unused(bytes);

  if (ec) {
    return fail(ec, "send");
  }
}
