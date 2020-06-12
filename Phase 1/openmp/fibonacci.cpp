#include <cstddef>
#include <iostream>

#include "omp.h"

constexpr std::size_t nb_threads{8};

std::size_t fib(std::size_t n) {
	if (n <  2) return n;
	if (n < 20) return fib(n - 2) + fib(n - 1);
	std::size_t x, y;
	#pragma omp task shared(x)
	x = fib(n - 2);
	#pragma omp task shared(y)
	y = fib(n - 1);
	#pragma omp taskwait
	return x + y;
}

int main() {
	std::size_t n;
	std::cin >> n;

	std::size_t result;
	#pragma omp parallel num_threads(nb_threads)
	{
		#pragma omp single
		result = fib(n);
	}
	std::cout << result << '\n';

	return 0;
}