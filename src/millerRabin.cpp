#include "millerRabin.h"

namespace {
//returns x^p % n, uses exponentiation by squaring
uint64_t powmod(uint64_t x, uint64_t p, uint64_t n) {
	
	if(p == 0) return 1;

	unsigned __int128 res = x;
	unsigned __int128 y = 1;

	while(p > 1) {
		if(p % 2 == 1) {
			y = (y * res) % n;
			p--;
		}
		
		res = (res*res) % n; //(x*x) % n <=> (x%n * x%n) % n
		p /= 2;
	}

	return (res*y)%n;
}
}

bool isPrime(uint64_t n, uint64_t* divisor) {

	const std::vector<unsigned> bases = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};

	if(n <= 1) return false;
	for(auto prime : bases) {
		if(n == prime) return true;
		if(n % prime == 0) {
			if(divisor!=nullptr) *divisor = prime;
			return false;
		}
	}

	//n - 1 = s^2 * d
	uint64_t s = 0;
	uint64_t d = n-1;
	while(d % 2 == 0) {
		s++;
		d /= 2;
	}

	uint64_t x, y;
	for(auto a : bases) {
		x = powmod(a, d, n);
		for(uint64_t i = 0; i < s; i++) {
			y = powmod(x, 2, n);
			if(y == 1 && x != 1 && x != n-1) {//in this case we also found one divisor of n, useful for later
				if(divisor!=nullptr) *divisor = std::gcd(x-1,n);
				return false; 
			}
			x = y;
		}
		if(y != 1) return false;
	}

	return true;
}

bool isPrime(int n) {
	if(n <= 1) return false;
	return isPrime((uint64_t)n);
}