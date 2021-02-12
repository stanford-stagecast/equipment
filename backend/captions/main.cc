#include "captioner.hh"
#include <unistd.h>
#include<string>

int main() {
	std::string not_used;
	Captioner captioner{"vp4u-e3zq-2xzu-1wgs-azkd", "test.txt"};
	std::cout << captioner.next_line();
	while (!captioner.eof() && std::cin) {
		getline(std::cin, not_used);
		captioner.send_next_caption();
		std::cout << captioner.next_line();
	}
	return 0;
}
