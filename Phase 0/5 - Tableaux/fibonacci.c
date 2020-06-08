#include <stdio.h>

unsigned int fib_array[64];
unsigned int fib_index = 0;

unsigned int fib(unsigned int n) {
	if (n <= fib_index)
		return fib_array[n];
	unsigned int a = fib_array[fib_index - 1];
	unsigned int b = fib_array[fib_index];
	for (unsigned int i = fib_index; i < n; i++) {
		fib_array[i + 1] = a + b;
		// Fast swap
		// b += a;
		// a -= b;
		// a = -a;

		// Alternate swap with memory (implemented in the _asm.s)
		a = b;
		b = fib_array[i + 1];
	}
	fib_index = n;
	return b;
}

int main() {
	fib_array[0] = 0;
	fib_array[1] = 1;
	fib_index = 1;

	// while (1) {
	// 	unsigned int n;
	// 	scanf("%u", &n);
	// 	printf("Fib(%u) : ", n);
	// 	printf("%u\n", fib(n));
	// }

	printf("%u\n", fib(10));
	printf("%u\n", fib(5));
	printf("%u\n", fib(5));
	printf("%u\n", fib(20));
	printf("%u\n", fib(10));

	return 0;
}