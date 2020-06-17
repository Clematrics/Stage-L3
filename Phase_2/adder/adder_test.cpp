#include <iostream>

#include "adder.hpp"

int main() {
	unsigned int a;
	unsigned int b;
	unsigned int r;

	Adder adder1(false);
	a = 10000;
	b = 20000;
	adder1.add(a, b, &r);
	std::cout << a << " + " << b << " is " << r << '\n';

	Adder adder2(true);
	a = 10000;
	b = 20000;
	adder2.add(a, b, &r);
	std::cout << a << " + " << b << " is " << r << '\n';

	return 0;
}
