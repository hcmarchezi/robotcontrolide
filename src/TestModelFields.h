#ifndef TESTMODELFIELDS_H
#define TESTMODELFIELDS_H

#include "Field.h"
#include<string>
#include<map>

using namespace std;

class TestModelFields
{
public: static void CompareFieldMaps(map<string,Field*> fieldMap1,map<string,Field*> * fieldMap2);
public: static void CompareFieldMapStringMap(map<string,Field*> fieldMap,map<string,string> * stringMap); 

};

#endif