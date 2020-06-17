#include "adder.hpp"

Adder::Adder(bool with_carry) {
	offset = with_carry ? 1 : 0;
}

void Adder::add(unsigned int a, unsigned int b, unsigned int* result) {
	*result = a + b + offset;
}
