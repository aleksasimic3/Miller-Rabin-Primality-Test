#ifndef _MILLER_RABIN_MP_H_
#define _MILLER_RABIN_MP_H_

#include <gmp.h>
#include <gmpxx.h>
#include <vector>
#include <mpreal.h>
#include <chrono>
#include <string>

#include "millerRabin.h"

//Multi precision overload of the Miller-Rabin primality test
bool isPrime(mpz_class n, bool deterministic=false); //add possible divisor

#endif