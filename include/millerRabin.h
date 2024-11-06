#ifndef _MILLER_RABIN_H_
#define _MILLER_RABIN_H_

#include <cstdint>
#include <vector>
#include <chrono>
#include <numeric>

#include <iostream>

//returns if n is prime or not, uses Miller Rabin primality test (deterministic)
//https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
bool isPrime(uint64_t n, uint64_t* divisor=nullptr); //accepts n <= 2^64 - 1
bool isPrime(int n);

#endif
