#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <mutex>
#include <condition_variable>

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;
using tcp = boost::asio::ip::tcp;
namespace pt = boost::property_tree;

using namespace std;

void validate_arguments(int argc) {
  if (argc != 5) {
    cout << "Usage: cli midi_device server port user" << endl;
    cout << "midi_device: /dev/midi*" << endl;
    cout << "server: hostname or IP of server" << endl;
    cout << "port: websocket port" << endl;
    cout << "user: user number in [0, 31]; 0 = operator, 1-31 = actor/musician" << endl;
    exit(0);
  }
}

void open_websocket(net::io_context& ioc, stream<tcp::socket>& ws, string server_name, string port) {
  cout << "Connecting to server..." << endl;
  tcp::resolver resolver{ioc};

  auto const lookup = resolver.resolve(server_name, port);
  auto endpoint = net::connect(ws.next_layer(), lookup);

  string host = server_name;
  host += ":";
  host += endpoint.port();

  cout << "Testing connection..." << endl;
  ws.handshake(host, "/");
  cout << "Connection Success!" << endl;
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

void open_midi(ifstream& midi, string device) {
  cout << "Connecting to MIDI controller" << endl;
  midi.open(device);
  cout << "MIDI controller connected!" << endl;
}

map<uint8_t, uint8_t> changes{};
mutex changes_mutex;
condition_variable ws_signal;

void ws_thread(string server_name, string port) {
  net::io_context ioc;
  stream<tcp::socket> ws(ioc);
  open_websocket(ioc, ws, server_name, port);

  // whenever changes are available, 
  while (true) {
    map<uint8_t, uint8_t> local;
    {
      unique_lock<mutex> lock(changes_mutex);
      if (changes.size() == 0) {
        ws_signal.wait(lock);
      }
      local = changes;
      changes.clear();
    }

    if (local.size() == 0) {
      continue;
    }
    pt::ptree json;
    json.put("type", "set-levels");
    pt::ptree array;
    for (auto x : local) {
      pt::ptree current;
      current.put("channel", x.first);
      current.put("value", x.second);
      array.push_back(std::make_pair("", current));
    }
    json.put_child("values", array);
    stringstream ss;
    pt::write_json(ss, json);
    string s = ss.str();
    ws.write(net::buffer(s));

    // only one update every 10ms
    this_thread::sleep_for(chrono::milliseconds(10));
  }
}

void midi_thread(string device, int user) {
  ifstream midi;
  open_midi(midi, device);
  cout << "Levels " << 16 * user << "-" << 16 * (user + 1) - 1 << " are being synchronized." << endl;

  // continuously get MIDI messages and send them to the other thread,
  // overwriting old messages if they haven't been read yet
  while (true) {
    uint8_t status, channel, value;
    read_midi_message(midi, status, channel, value);
    if (status != 0xB0) {
      cout << "Unknown MIDI message status: " << status << endl;
    }
    uint8_t output_channel = 16 * user;
    if (channel < 8) {
      output_channel += channel;
    } else if (channel >= 16 && channel < 24) {
      output_channel += channel - 8;
    } else {
      continue;
    }

    unique_lock<mutex> lock(changes_mutex);
    changes[output_channel] = value * 2;
    ws_signal.notify_all();
  }
}

int main(int argc, char **argv) {
  validate_arguments(argc);

  string device = argv[1];
  string server_name = argv[2];
  string port = argv[3];
  int user = stoi(argv[4]);
  cout << "Using MIDI device: <" << device << ">" << endl;
  cout << "Using server: <" << server_name << ":" << port << ">" << endl;
  cout << "Using user: <" << user << ">" << endl;

  thread ws(ws_thread, server_name, port);
  thread midi(midi_thread, device, user);

  ws.join();
  midi.join();
}
