#include <iostream>

#include "omp.h"

int main() {
	constexpr std::size_t nb_threads {8};

	#pragma omp parallel num_threads(nb_threads)
	{
		std::cout << "Hello ";
		std::cout << "world from thread " << omp_get_thread_num() << '\n';
	}

	return 0;
}