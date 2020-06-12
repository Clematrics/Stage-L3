#include <cstddef>
#include <iostream>
#include <vector>

#include "omp.h"

int main() {
	constexpr std::size_t size {268435456};
	constexpr std::size_t nb_threads {8};
	std::vector<std::uint64_t> array(size);

	#pragma omp parallel for num_threads(nb_threads) schedule(static)
	for (std::size_t i = 0; i < size; i++) {
		array[i] = i;
	}

	std::size_t sum {0};
	#pragma omp parallel for num_threads(nb_threads) schedule(static) reduction (+:sum)
	for (std::size_t i = 0; i < size; i++) {
		sum += array[i];
	}

	std::cout << "Sum is " << sum << '\n';
	return 0;
}