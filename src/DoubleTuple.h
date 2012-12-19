
#ifndef DOUBLETUPLE_H
#define DOUBLETUPLE_H

struct DoubleTuple
{
  double x, y;
};

// Create a zero vector
DoubleTuple DoubleTupleZero();

// Create a vector from the given values
DoubleTuple DoubleTupleSet(double x, double y);

#endif


