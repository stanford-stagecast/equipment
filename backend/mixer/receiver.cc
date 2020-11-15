#include "receiver.hh"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>


using namespace std;

Receiver::Receiver(uint16_t port): receiver_thread_(&Receiver::receive, this, port) {}

std::optional<Receiver::Output> Receiver::read() {
  unique_lock<mutex>lock (mutex_);
  return current_;
}

struct __attribute__ ((packed)) dmx_msg {
  char magic[10]; // "Stagecast"
  uint64_t seqno;
  uint16_t universe;
  uint8_t data[512];
};
static_assert(sizeof(dmx_msg) == 532);

void Receiver::receive(uint16_t port) {
  (void) port;
  int sock;
  while ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    cerr << "Error creating DMX socket." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;
  while (bind(sock, reinterpret_cast<const sockaddr *>(&server), sizeof(server)) < 0) {
    cerr << "Error binding DMX socket." << endl;
  }

  dmx_msg msg;
  cout << "Listening for DMX." << endl;
  while (true) {
    ssize_t len = recv(sock, &msg, sizeof(dmx_msg), 0);

    if (len != sizeof(dmx_msg)) continue;
    if (msg.magic[9] != 0) continue;
    if (strcmp(msg.magic, "Stagecast")) continue;
    if (msg.seqno <= last_seqno_) continue;

    last_seqno_ = msg.seqno;

    Output output;
    for (size_t user = 0; user < 32; user++) {
      for (size_t channel = 0; channel < 8; channel++) {
        size_t level = user * 16 + channel;
        size_t pan = level + 8;
        output[user][channel].pan = msg.data[pan];
        output[user][channel].level = msg.data[level];
      }
    }

    {
      unique_lock<mutex> lock(mutex_);
      current_ = output;
    }
  }
}
