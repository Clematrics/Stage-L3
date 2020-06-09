#include <stdio.h>

unsigned int fib_internal(unsigned int n, unsigned int a, unsigned int b) {
	return n ? fib_internal(n - 1, b, a+b) : a;
}

unsigned int fib(unsigned int n) {
	return fib_internal(n, 0, 1);
	// if (n == 0) return 0;
	// if (n == 1) return 1;
	// unsigned int a = fib(n - 1);
	// unsigned int b = fib(n - 2);
	// return a + b;
}

int main() {
	unsigned int i;
	scanf("%u", &i);

	printf("%u\n", fib(i));

	return 0;
}