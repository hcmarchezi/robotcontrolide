
#ifndef CONVERSION_H
#define CONVERSION_H

#ifdef WIN32
#include <windows.h>
#endif

#include <string>
#include "Color.hh"
#include "Vector.hh"
#include "DoubleTuple.h"
#include "IntegerTuple.h"

using namespace std;

class Conversion
{
// Metodos para coverter cada um dos tips em string
public: static string BoolToString(bool value);
public: static string IntegerToString(int value);
public: static string LongToString(long value);
public: static string DoubleToString(double value);
public: static string GzVectorToString(GzVector position);
public: static string GzColorToString(GzColor color);
public: static string DoubleTupleToString(DoubleTuple tuple);
public: static string IntegerTupleToString(IntegerTuple tuple);

// Metodos para converter a string em cada um dos tipos
public: static bool StringToBool(string value);
public: static int  StringToInt(string value);
public: static long StringToLong(string value);
public: static double StringToDouble(string value);
public: static GzVector StringToGzVector(string value);
public: static GzColor StringToGzColor(string value);
public: static DoubleTuple StringToDoubleTuple(string value);
public: static IntegerTuple StringToIntegerTuple(string value);

};

#endif
