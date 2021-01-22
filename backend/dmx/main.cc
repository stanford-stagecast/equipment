#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

struct __attribute__ ((packed)) udp_dmx_msg {
  char magic[10]; // "Stagecast"
  uint64_t seqno;
  uint16_t universe;
  uint8_t data[512];
};
static_assert(sizeof(udp_dmx_msg) == 532);

void validate_arguments(int argc) {
  if (argc != 3 && argc != 4) {
    cout << "Usage: dmx dmx_device port [universe]" << endl;
    cout << "dmx_device: /dev/ttyUSB*" << endl;
    cout << "port: UDP port to listen on" << endl;
    cout << "universe: DMX universe, zero-based (default 0)" << endl;
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

int open_udp_server(int port) {
  sockaddr_in localaddr;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) return fd;
  memset(&localaddr, 0, sizeof(localaddr));
  localaddr.sin_family = AF_INET;
  localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localaddr.sin_port = htons(port);
  if (int x = bind(fd, reinterpret_cast<sockaddr *>(&localaddr), sizeof(localaddr)) < 0) {
    return x;
  }
  return fd;
}

int main(int argc, char **argv) {
  validate_arguments(argc);

  string device = argv[1];
  int port = stoi(argv[2]);
  int universe = 0;
  if (argc == 4) {
    universe = stoi(argv[3]);
  }
  cout << "Using DMX device: <" << device << ">" << endl;
  cout << "Using port: <" << port << ">" << endl;
  cout << "Using universe: <" << universe << ">" << endl;

  ofstream dmx(device);

  int fd = open_udp_server(port);

  uint8_t buf[1024];
  uint64_t seqno = 0;

  for (;;) {
    sockaddr_in remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    int recvlen = recvfrom(fd, buf, sizeof(buf), 0, reinterpret_cast<sockaddr *>(&remoteaddr), &addrlen);
    if (recvlen != sizeof(udp_dmx_msg)) {
      cout << "Invalid message length: " << recvlen << endl;
      continue;
    }
    udp_dmx_msg *msg = reinterpret_cast<udp_dmx_msg *>(&buf);
    if (strncmp(msg->magic, "Stagecast", 10) != 0) {
      cout << "Invalid magic word.  Check byte ordering?" << endl;
      continue;
    }
    if (msg->universe != universe) continue;
    if (msg->seqno < seqno) continue;
    seqno = msg->seqno;

    serial_dmx_msg out;
    out.start_of_message = 0x7e; // start
    out.label = 6; // dmx output
    out.length_lsb = 513 & 0xff;
    out.length_msb = (513 >> 8) & 0xff;
    out.data[0] = 0; // start byte
    memcpy(&out.data[1], msg->data, sizeof(msg->data)); // data
    out.end_of_message = 0xe7; // end

    dmx.write(reinterpret_cast<char *>(&out), sizeof(out));
    dmx.flush();
  }
}
