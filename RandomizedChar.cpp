#include "RandomizedChar.h"

#include <random>

using namespace std;

const char RandomizedChar::characterLookupTable[]{
	'a', 'a', 'a', 'a', 'a',
	'e', 'e', 'e', 'e', 'e',
	'i', 'i', 'i', 'i', 'i',
	'o', 'o', 'o', 'o', 'o',
	'u', 'u', 'u', 'u', 'u',
	'b', 'c', 'd', 'f', 'g', 'h', 'j',
	'k', 'l', 'm', 'n', 'p', 'q', 'r',
	's', 't', 'v', 'w', 'x', 'y', 'z' };

const size_t RandomizedChar::characterLookupTableSz{
	sizeof(RandomizedChar::characterLookupTable) / sizeof(RandomizedChar::characterLookupTable[0]) };

RandomizedChar::RandomizedChar()
	: dist(0, characterLookupTableSz - 1) {
	rng.seed(random_device()());
}
char RandomizedChar::randomChar() {
	int randomIndex = dist(rng);
	return characterLookupTable[randomIndex];
}
