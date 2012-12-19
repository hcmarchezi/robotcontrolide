
#include "IntegerTuple.h"

// Create a zero vector
IntegerTuple IntegerTupleZero()
{
	IntegerTuple vector;
	vector.x = 0;
	vector.y = 0;
	return vector;
}

// Create a vector from the given values
IntegerTuple IntegerTupleSet(int x, int y)
{
	IntegerTuple vector;
	vector.x = x;
	vector.y = y;
	return vector;
}