#include "millerRabinMP.h"

namespace {
mpz_class powmod(mpz_class x, mpz_class p, mpz_class n) {
	
	if(p == 0) return mpz_class(1);

	mpz_class y(1);

	while(p > 1) {
		if(p % 2 == 1) {
			y = (y * x) % n;
			p--;
		}
		
		x = (x*x) % n; //(x*x) % n <=> (x%n * x%n) % n
		p /= 2u;
	}

	return (x*y)%n;
}

bool millerRabinCore(mpz_class s, mpz_class d, mpz_class a, mpz_class n) { //template

	mpz_class x, y;

	x = powmod(a, d, n);

	for(mpz_class i(0); i < s; i++) {
		y = powmod(x, 2, n);
		if(y == 1 && x != 1 && x != n-1) return false; //found non trivial divisor of n
		x = y;
	}

	if(y != 1) return false;

	return true;
}
}

bool isPrime(mpz_class n, bool deterministic) {

	const std::vector<unsigned> bases = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};
	const mpz_class magicNumber = mpz_class("3317044064679887385961981"); //no pseudoprimes smaller than magicNumber for above bases
	const unsigned numberOfIterations = 100; //probability of error: 4^(-100) ~ 6.22 * 10^(-61)
											//number of stars in the observable universe ~ 2 * 10^23
											//number of grains of sand on Earth ~ 7.5 * 10^18

	if(n <= 1) return false;

	//use fundamental types when n < 2^64
	if(n < mpz_class("18446744073709551616")) {
		uint64_t n_uint = (uint64_t)std::stoull(n.get_str(), nullptr, 10);
 		return isPrime(n_uint);
	}

	
	for(auto prime : bases) {
		if(n == prime) return true;
		if(n % prime == 0) { //found non trivial divisor
			return false;
		}
	}

	//n - 1 = 2^s * d
	mpz_class s(0);
	mpz_class d(n-1);
	while(d % 2 == 0) {
		s = s + 1;
		d /= 2u; //mpz_class only has overload for unsgned ints
	}	

	if(n < magicNumber) { //if n is smaller than magic number than we only need to check a small set of bases

		for(auto a : bases) {
			if(!millerRabinCore(s, d, a, n)) return false;
			if(a >= n-1) break;
		}
		return true;

	}
	else {

		if(deterministic) {
			//upperBound = floor(2*(ln(n))^2)
			mpz_class upperBound = mpz_class(mpfr::floor(mpfr::pow(mpfr::log(mpfr::mpreal(n.get_str())), 2u) * 2).toString());
			std::cout << "Upper bound: " << upperBound.get_str() << '\n';
			for(mpz_class a(2); a < upperBound; a++) {
				if(!millerRabinCore(s, d, a, n)) return false;
			}
			return true;
		}
		else {
			gmp_randclass rand(gmp_randinit_default);
			rand.seed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			for(unsigned i = 0; i < numberOfIterations; i++) {
				mpz_class a = rand.get_z_range(n-4) + 2; //a from interval (2, n-2)
				if(!millerRabinCore(s, d, a, n)) return false;
			}
			return true;
		}
	}	

	return true;
}