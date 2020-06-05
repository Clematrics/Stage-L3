#include <iostream>
#include <cstdio>

#include "decode.hpp"

int main() {
	RawInstruction raw;
	while (true) {
		// std::cin >> raw;
		scanf("%x", &raw);
		Instruction instr = decode(raw);
		std::cout << instr;
	}
	return 0;
}