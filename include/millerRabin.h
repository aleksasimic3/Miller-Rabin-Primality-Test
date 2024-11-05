#ifndef _MILLER_RABIN_H_
#define _MILLER_RABIN_H_

#include <cstdint>
#include <vector>
#include <chrono>
#include <numeric>

#include <iostream> //temp

//maybe rename this from millerRabin.h to primalityTest.h

//returns if n is prime or not, uses Miller Rabin primality test (deterministic)
//https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
//large primes http://www.isthe.com/chongo/tech/math/prime/mersenne.html
//miller rabin test sometimes finds one divisor
bool isPrime(uint64_t n, uint64_t* divisor=nullptr); //accepts n <= 2^64 - 1
bool isPrime(int n);

//bool isPrime(unsigned _int128 n); //check if _int128 is defined

#endif