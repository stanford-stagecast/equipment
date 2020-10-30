#include "transmitter.hh"

#include <iostream>
#include <boost/core/ignore_unused.hpp>
#include <boost/bind.hpp>

#define RETX_TIME_MS 1000
#define ARTNET_PORT 0x1936

Transmitter::Transmitter(net::io_context &context, net::ip::address destination,
                         unsigned universe)
    : ioc_(context), destination_(destination), socket_(context),
      endpoint_(destination, ARTNET_PORT), universe_(universe) {
  if (destination.is_v4()) {
    std::cout << "Sending Universe " << universe << " Art-Net to <"
              << destination << ">" << std::endl;
    socket_.open(net::ip::udp::v4());
  } else if (destination.is_v6()) {
    std::cout << "Sending Universe " << universe << " Art-Net to <"
              << destination << ">" << std::endl;
    socket_.open(net::ip::udp::v6());
  } else {
    std::cerr << "Cannot send Universe " << universe << " Art-Net to <"
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

std::array<uint8_t, 530> serialize_packet(unsigned universe,
                                          Transmitter::universe_t levels,
                                          uint8_t seqno) {
  // Format based on:
  // https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/.

  // This doesn't implement the full specification, just a very specific
  // version of the DMX packet format.

  std::array<uint8_t, 530> packet;
  // Magic Number/ID
  packet[0] = 'A';
  packet[1] = 'r';
  packet[2] = 't';
  packet[3] = '-';
  packet[4] = 'N';
  packet[5] = 'e';
  packet[6] = 't';
  packet[7] = 0;
  packet[8] = 0x00;  // OpCode - Low
  packet[9] = 0x50;  // OpCode - High
  packet[10] = 0x00; // Protocol Revision - Low
  packet[11] = 0x00; // Protocol Revision - High
  packet[12] = seqno;
  packet[13] = 0; // Physical input port
  packet[14] = (universe & 0xff); // Universe - Low
  packet[15] = ((universe >> 8) & 0xff); // Universe - High
  packet[16] = 1; // Length of Array (High)
  packet[17] = 0; // Length of Array (Low)
  for (int i = 0; i < 512; i++) {
    packet[18 + i] = levels[i];
  }
  return packet;
}

void Transmitter::transmit() {
  ms_since_last_tx_ = 0;
  auto buffer = net::buffer(serialize_packet(universe_, saved_, seqno_));

  socket_.async_send_to(buffer, endpoint_, boost::bind(&Transmitter::on_send, this, net::placeholders::error, net::placeholders::bytes_transferred));

  seqno_++; // increment (and/or overflow)
  if (seqno_ == 0) seqno_++; // skip 0
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
