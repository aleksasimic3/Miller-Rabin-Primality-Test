#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <string>
#include <climits>
#include <random>
#include <utility>
#include <gmp.h>
#include <gmpxx.h>

#include "millerRabin.h"
#include "millerRabinMP.h"

#define DEFAULT_LIMIT 10000u
#define POW_2_1000 "10715086071862673209484250490600018105614048117055336074437503883703510511249361224931983788156958581275946729175531468251871452856923140435984577574698574803934567774824230985421074605062371141877954182153046474983581941267398767559165543946077062914571196477686542167660429831652624386837205668069376"
#define POW_2_64 "18446744073709551616"

//add error checking and handling
//add number of random tests from command line

//validity tests
bool knownPrimesUintTest();
bool knownPrimesMpzTest();
bool mersennePrimesTest();

//bool knownCompositesUintTest(); //combine primes from above to make composite numbers
//bool knownCompositesMpzTest();

//speed tests
bool randomUintTest();
bool randomMpzTest();
bool dragRace(); //todo

int main(int argc, char* argv[]) {
	std::cout << '\n';

	//uint64_t known primes
	if(knownPrimesUintTest() == false) {
		std::cout << "Test stopped on error.\n";
		return -1;
	}

	//mpz_class known primes
	if(knownPrimesMpzTest() == false) {
		std::cout << "Test stopped on error.\n";
		return -1;
	}

	//uint64_t random numbers up to 2^64
	randomUintTest();

	//mpz_class random numbers up to 2^1000
	randomMpzTest();

	//Mersenne primes speed validity and speed test
	if(mersennePrimesTest() == false) {
		std::cout << "Test stopped on error.\n";
		return -1;
	}

	return 0;
}

bool knownPrimesUintTest() {

	std::cout << "(uint64_t) Testing known primes: ";

	std::vector<uint64_t> numbers;
	std::fstream in_file("uint64_primes.txt"); //only works from root dir
	if(in_file.is_open() == false) {
		std::cout << "ERROR\n\tFile not found! Run test.exe from 'test' directory.\n";
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
	std::fstream in_file("mpz_primes.txt");
	if(in_file.is_open() == false) {
		std::cout << "ERROR\n\tFile not found! Run test.exe from 'test' directory.\n";
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
	std::cout << "(uint64_t) Testing Mersenne primes:\n";

	std::fstream inputFile("mersenne_primes_small.txt");
	if(inputFile.is_open() == false) {
		std::cout << "ERROR\n\tFile not found! Run test.exe from 'test' directory.\n";
		return false;		
	}
	std::string inputLine;
	while(std::getline(inputFile, inputLine)) {
		auto delimiterIndex = inputLine.find(',');
		std::string powerTwoNotation = inputLine.substr(0, delimiterIndex);
		uint64_t value = std::stoull(inputLine.substr(delimiterIndex+1));
		std::cout << "\t" << powerTwoNotation << ": ";

		auto startTime = std::chrono::high_resolution_clock::now();

		bool result = isPrime(value);

		auto endTime = std::chrono::high_resolution_clock::now();
		auto timeElapsed = (endTime - startTime) / std::chrono::milliseconds(1);

		if(result == false) {
			std::cout << "ERROR!\n";
			return false;
		}
		std::cout << "OK (Calculated in " << timeElapsed << "ms)\n";
	}
	inputFile.close();

	//mpz_class for large primes up to 2^4500
	std::cout << "(mpz_class) Testing large Mersenne primes:\n";

	inputFile.open("mersenne_primes_large.txt");
	if(inputFile.is_open() == false) {
		std::cout << "ERROR\n\tFile not found! Run test.exe from 'test' directory.\n";
		return false;
	}
	while(std::getline(inputFile, inputLine)) {
		auto firstDelimiterIndex = inputLine.find(',');
		auto secondDelimiterIndex = inputLine.find(',', firstDelimiterIndex+1);
		std::string powerTwoNotation = inputLine.substr(0, firstDelimiterIndex);
		std::string numberOfDigits = inputLine.substr(firstDelimiterIndex+1, secondDelimiterIndex-firstDelimiterIndex-1);
		mpz_class value(inputLine.substr(secondDelimiterIndex+1));
		std::cout << "\t" << powerTwoNotation << ": ";

		auto startTime = std::chrono::high_resolution_clock::now();

		bool result = isPrime(value);

		auto endTime = std::chrono::high_resolution_clock::now();
		auto timeElapsed = (endTime - startTime) / std::chrono::milliseconds(1);

		if(result == false) {
			std::cout << "ERROR!\n";
			return false;
		}		
		std::cout << "OK (Calculated in " << timeElapsed << "ms)\n";
	}
	inputFile.close();

	return true;
}

bool dragRace() {

	return true;
}
