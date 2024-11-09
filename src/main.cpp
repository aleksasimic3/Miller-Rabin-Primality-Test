#include <iostream>
#include <chrono>
#include <gmp.h>
#include <gmpxx.h>
#include <string>

#include "millerRabinMP.h"
#include "millerRabin.h"

int main(int argc, char* argv[]) {

	auto startTime = std::chrono::high_resolution_clock::now();

	//handle multiple numbers from the command line

	mpz_class n;
	if(argc > 1) {
		try {
			n = mpz_class(std::string(argv[1]));
		}
		catch (...) {
			std::cout << "Invalid input!\n";
			return -1;
		}
	}
	else {
		std::cout << "Input number as command line argument.\n";
		return -1;
	}

	bool res = isPrime(n, false);

	auto endTime = std::chrono::high_resolution_clock::now();
	auto timeElapsed = endTime - startTime;

	std::cout << '\n' << n.get_str() << (res ? " is prime!" : " is not prime!") << "\n";
	std::cout << "\nCalculated in " << (timeElapsed / std::chrono::milliseconds(1)) << "ms\n";

	return 0;
}