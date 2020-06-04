#include <stdio.h>

int main() {
	int seed = 0x533DC0DE;
	int multiplier = 0x41C64E6D;
	int increment = 0x2039;

	int i = seed;
	i = multiplier * i + increment;
	printf("%d\n", i % 10);
	i = multiplier * i + increment;
	printf("%d\n", i % 10);
	i = multiplier * i + increment;
	printf("%d\n", i % 10);

	return 0;
}