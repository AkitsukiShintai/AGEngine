#include <iostream>

void print_bits(const int& value, const int& size) {
	for (int i = size - 1; i >= 0; --i) {
		std::cout << (((1 << i) & value) ? "1" : "0");
		//std::cout << ( (1<<i) & value) ? "1":"0"; //wrong
	}
	std::cout << std::endl;
}

int main() {
	int size = 3;
	int num_total = 1 << size;
	for (int i = 0; i < num_total; ++i) {
		int half = i >> 1; //integer division uses floor for positive numbers
		int gray_code = i ^ half;
		print_bits(gray_code, size);
	}
	return 0;
}