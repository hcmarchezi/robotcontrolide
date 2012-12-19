#include "DoubleTuple.h"

// Create a zero vector
DoubleTuple DoubleTupleZero()
{
	DoubleTuple vector;
	vector.x = 0.0;
	vector.y = 0.0;
	return vector;
}

// Create a vector from the given values
DoubleTuple DoubleTupleSet(double x, double y)
{
	DoubleTuple vector;
	vector.x = x;
	vector.y = y;
	return vector;
}