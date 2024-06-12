#pragma once
#include <random>;


class RandomizedChar {
	static const char characterLookupTable[];
	static const size_t characterLookupTableSz;

	std::mt19937 rng;
	std::uniform_int_distribution<int> dist;
public:
	RandomizedChar();
	char randomChar();
};

