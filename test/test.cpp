#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <string>
#include <climits>
#include <random>
#include <gmp.h>
#include <gmpxx.h>

#include "millerRabin.h"
#include "millerRabinMP.h"

#define DEFAULT_LIMIT 10000u
#define POW_2_1000 "10715086071862673209484250490600018105614048117055336074437503883703510511249361224931983788156958581275946729175531468251871452856923140435984577574698574803934567774824230985421074605062371141877954182153046474983581941267398767559165543946077062914571196477686542167660429831652624386837205668069376"
#define POW_2_64 "18446744073709551616"

//validity tests
bool knownPrimesUintTest();
bool knownPrimesMpzTest();
bool mersennePrimesTest(); //todo

//speed tests
bool randomUintTest();
bool randomMpzTest(); //todo
bool dragRace(); //todo

//not very readable but works for crude testing
int main(int argc, char* argv[]) {
	std::cout << '\n';

	//uint64_t known primes
	if(knownPrimesUintTest() == false) {
		std::cout << "\nTest stopped on error.\n";
		//return -1;
	}

	//mpz_class known primes
	if(knownPrimesMpzTest() == false) {
		std::cout << "\nTest stopped on error.\n";
		//return -1;
	}

	//uint64_t random numbers up to 2^64
	randomUintTest();

	//mpz_class random numbers up to 2^1000
	randomMpzTest();

	/*
	//mpz random numbers (up to 2^3127)
	std::cout << "\nTesting " << limit << " random mpz_class numbers up to " << DEFAULT_UPPERBOUND_MPZ_SHORT_NOTATION << ": ";
	mpz_class upperBound = mpz_class(DEFAULT_UPPERBOUND_MPZ);
	gmp_randclass rand(gmp_randinit_default); //randinit_default generates numbers with similar digits to n, that probably throws off the error rate
	rand.seed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	prime_numbers = 0;
	composite_numbers = 0;
	divisors = 0;
	startTime = std::chrono::high_resolution_clock::now();
	for(uint64_t i = 0; i < limit; i++) {
		mpz_class n = rand.get_z_range(upperBound);
		//std::cout << n << '\n';
		if(isPrime(n)) prime_numbers++;
		else composite_numbers++;
	}
	endTime = std::chrono::high_resolution_clock::now();
	timeElapsed = endTime - startTime;
	std::cout << "OK\n\tFound:\n\t\tPrimes: " << prime_numbers << "\n\t\tComposites: " << composite_numbers << '\n';
	std::cout << "Calculated in: " <<  (timeElapsed / std::chrono::milliseconds(1)) << "ms\n";	

	//mersene primes with separate timing

	*/

	return 0;
}

bool knownPrimesUintTest() {

	std::cout << "(uint64_t) Testing known primes: ";

	std::vector<uint64_t> numbers;
	std::fstream in_file("./test/uint64_primes.txt"); //only works from root dir
	if(in_file.is_open() == false) {
		std::cout << "ERROR\n\tFile not found!\n";
		return false;
	}
	uint64_t x;
	while(in_file >> x) numbers.push_back(x);
	in_file.close();

	auto startTime = std::chrono::high_resolution_clock::now();
	for(auto n : numbers) {
		if(isPrime(n) == false) {
			std::cout << "ERROR\n\t" << n << " is prime, but test returned composite.\n";
			return false;
		}
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	auto timeElapsed = (endTime - startTime) / std::chrono::milliseconds(1);

	std::cout << "OK (Checked " << numbers.size() << " numbers in " << timeElapsed << "ms)\n";

	return true;
}

bool knownPrimesMpzTest() {

	std::cout << "(mpz_class) Testing known primes: ";

	std::vector<mpz_class> numbers;
	std::fstream in_file("./test/mpz_primes.txt");
	if(in_file.is_open() == false) {
		std::cout << "ERROR\n\tFile not found!\n";
		return false;
	}
	std::string s;
	while(in_file >> s) numbers.push_back(mpz_class(s));
	in_file.close();

	auto startTime = std::chrono::high_resolution_clock::now();
	for(auto n : numbers) {
		if(isPrime(n) == false) {
			std::cout << "ERROR\n\t" << n.get_str() << " is prime, but test returned composite.\n";
			return false;
		}
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	auto timeElapsed = (endTime - startTime) / std::chrono::milliseconds(1);

	std::cout << "OK (Checked " << numbers.size() << " numbers in " << timeElapsed << "ms)\n";

	return true;
}

bool randomUintTest() {

	std::cout << "(uint64_t) Testing random numbers up to 2^64: ";

	unsigned limit = DEFAULT_LIMIT;
	std::random_device rd;
	std::mt19937_64 e2(rd());
	std::uniform_int_distribution<uint64_t> dist(0, ULLONG_MAX); //generates large numbers but ok for now
	unsigned primes = 0;
	unsigned composites = 0;
	unsigned divisors = 0;
	uint64_t possibleDivisor;

	auto startTime = std::chrono::high_resolution_clock::now();
	for(unsigned i = 0; i < limit; i++) {
		possibleDivisor = 0;
		if(isPrime(dist(e2), &possibleDivisor)) primes++; //maybe move random generation outside of timed part
		else {
			composites++;
			if(possibleDivisor != 0) divisors++;
		}
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	auto timeElapsed = (endTime - startTime) / std::chrono::milliseconds(1);

	std::cout << "OK (Checked " << limit << " numbers in " << timeElapsed << "ms)\n";
	std::cout << "\tPrimes: " << primes << "\n\tComposites: " << composites << "\n\tDivisors: " << divisors << '\n';	

	return true;
}

bool randomMpzTest() {

	std::cout << "(mpz_class) Testing random numbers up to 2^1000: ";

	unsigned limit = DEFAULT_LIMIT;
	mpz_class upperBound = mpz_class(POW_2_1000);
	gmp_randclass rand(gmp_randinit_default); //randinit_default generates numbers with similar number of digits to n, is that ok?
	rand.seed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());	
	unsigned primes = 0;
	unsigned composites = 0;
	//unsigned divisors = 0;

	auto startTime = std::chrono::high_resolution_clock::now();
	for(unsigned i = 0; i < limit; i++) {
		mpz_class n = rand.get_z_range(upperBound);
		//std::cout << '\n' << n.get_str() << '\n';
		if(isPrime(n)) primes++;
		else composites++;
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	auto timeElapsed = (endTime - startTime) / std::chrono::milliseconds(1);

	std::cout << "OK (Checked " << limit << " numbers in " << timeElapsed << "ms)\n";
	std::cout << "\tPrimes: " << primes << "\n\tComposites: " << composites << '\n';	

	return true;
}

bool mersennePrimesTest() {

	//uint64_t for small primes up to 2^64

	//mpz_class for large primes up to 2^4500

	return true;
}

bool dragRace() {

	return true;
}