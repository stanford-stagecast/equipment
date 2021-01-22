#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>


using namespace std;

void validate_arguments(int argc) {
  if (argc < 4 || argc % 2 != 0) {
    cout << "Usage: forwarder in_port out_host_1 out_port_1 [out_host_2 out_port_2 ...]" << endl;
    exit(0);
  }
}

void read_midi_message(ifstream& midi, uint8_t& status, uint8_t& channel, uint8_t& value) {
  do {
    do {
      midi >> status;
      // we might get out of sync, so we need to wait for a control byte
    } while (status < 0x80);
    midi >> channel;
    midi >> value;
    // sometimes a byte will go missing and we'll get out of sync
  } while (channel >= 0x80 || value >= 0x80);
}

struct __attribute__ ((packed)) serial_dmx_msg {
  uint8_t start_of_message;
  uint8_t label;
  uint8_t length_lsb;
  uint8_t length_msb;
  uint8_t data[513];
  uint8_t end_of_message;
};
static_assert(sizeof(serial_dmx_msg) == 518);

int open_udp_port(int port) {
  sockaddr_in localaddr;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) return fd;
  memset(&localaddr, 0, sizeof(localaddr));
  localaddr.sin_family = AF_INET;
  localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localaddr.sin_port = htons(port);
  int one = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  if (int x = bind(fd, reinterpret_cast<sockaddr *>(&localaddr), sizeof(localaddr)) < 0) {
    return x;
  }
  return fd;
}

int main(int argc, char **argv) {
  validate_arguments(argc);

  int listen_port = stoi(argv[1]);
  vector<sockaddr_in> hosts;
  for (int i = 1; i < argc/2; i++) {
    string addr_s = argv[2 * i];
    unsigned port = stoi(argv[2 * i + 1]);
    struct sockaddr_in dst;

    dst.sin_family = AF_INET;
    dst.sin_port = htons(port);
    inet_aton(addr_s.c_str(), reinterpret_cast<in_addr *>(&dst.sin_addr.s_addr));
    hosts.push_back(dst);
  }

  int in_fd = open_udp_port(listen_port);
  int out_fd = open_udp_port(0);

  uint8_t buf[1024];

  for (;;) {
    sockaddr_in remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    int recvlen = recvfrom(in_fd, buf, sizeof(buf), 0, reinterpret_cast<sockaddr *>(&remoteaddr), &addrlen);
    if (recvlen == sizeof(buf)) {
      cout << "Message too large: " << recvlen << endl;
      continue;
    }
    for (const sockaddr_in& dst : hosts) {
      int x = sendto(out_fd, buf, recvlen, 0, reinterpret_cast<const sockaddr *>(&dst), sizeof(sockaddr_in));
      if (x < 0) {
        cerr << "Error, only sent " << x << " of " << recvlen << " bytes." << endl;
      }
    }
  }
}
