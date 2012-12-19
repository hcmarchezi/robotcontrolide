
#ifndef INTEGERTUPLE_H
#define INTEGERTUPLE_H

struct IntegerTuple
{
  int x, y;
};

// Create a zero vector
IntegerTuple IntegerTupleZero();

// Create a vector from the given values
IntegerTuple IntegerTupleSet(int x, int y);

#endif


