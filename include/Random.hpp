/*
 * Copyright (c) 2012 Christopher M. Baker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <assert.h>

#include "StackTrace.hpp"

namespace nitrus {

/**
 * A static class that provides functions for generating a sequence of random numbers.
 */
class Random {
public:

	/**
	 * Seeds the random number generator.
	 * The seed determines the sequence of the random numbers.
	 * If the random number generator is seeded twice with same value, the same number sequence will be produced.
	 * The random number generator seed is typically initialized to time(NULL) to prevent the same sequence from being produced every run.
	 *
	 * @param seed The seed value used to generate a number sequence.
	 */
	static void Seed(unsigned int seed) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		srand(seed);
	}

	/**
	 * Creates a uniformly distributed random number in the interval [0, 1).
	 *
	 * @return A uniform random number.
	 */
	static double Uniform() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return ((double) rand()) / RAND_MAX;
	}

	/**
	 * Creates a uniformly distributed random number in the interval [lower, upper).
	 *
	 * @param lower The inclusive left bound of the interval.
	 * @param upper The exclusive right bound of the interval.
	 * @return A uniform random number.
	 */
	static double Uniform(double lower, double upper) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (lower > upper) {
			// the caller accidentally reversed the bounds so we will fix it for them
			return Uniform(upper, lower);
		}

		return lower + (upper - lower) * Uniform();
	}

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		// seeding with the same value produces the same number sequence
		Seed(1234);
		double value = Uniform();
		Seed(1234);
		assert(value == Uniform());

		// two consecutive random numbers from the same seed will not be equal
		assert(Uniform() != Uniform());
	}
};

}

#endif /* RANDOM_HPP_ */
