#ifndef __RECEIVER_HH__
#define __RECEIVER_HH__

#include <string>
#include <array>
#include <optional>
#include <thread>
#include <stdint.h>
#include <mutex>
#include "levels.hh"

class Receiver {
public:
  typedef std::array<Level, 8> User;
  typedef std::array<User, 32> Output;
  Receiver(uint16_t port);
  std::optional<Output> read();
private:
  std::optional<Output> current_{std::nullopt};
  std::mutex mutex_{};
  std::thread receiver_thread_;
  uint64_t last_seqno_{0};
  void receive(uint16_t port);
};

#endif
