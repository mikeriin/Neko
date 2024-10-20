#ifndef RANDOM_NUMBER_H
#define RANDOM_NUMBER_H


#include "Neko.h"

#include <random>


static i32 RngFromRange(i32 min, i32 max)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);

	return dist(rng);
}


#endif // !RANDOM_NUMBER_H

