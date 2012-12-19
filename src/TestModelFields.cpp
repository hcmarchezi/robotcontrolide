
#include "TestModelFields.h"

void TestModelFields::CompareFieldMaps(map<string,Field*> fieldMap1,map<string,Field*> * fieldMap2)
{
  if ((fieldMap1).size() != (*fieldMap2).size())
  {
    throw new SystemException("Mapas não são do mesmo tamanho!","TestModelFields::CompareFieldMaps");
  }

  map<string,Field*>::iterator iterator1;
  for (iterator1 = (fieldMap1).begin(); iterator1 != (fieldMap1).end(); iterator1++)
  {
    string strFieldName = (*iterator1).first;
    Field * field1 = fieldMap1[ strFieldName ];
    Field * field2 = (*fieldMap2)[ strFieldName ];
    string strFieldValue1 = field1->ToString();
    string strFieldValue2 = field2->ToString();
    string strFieldType1 = field1->GetTypeName();
    string strFieldType2 = field2->GetTypeName();
    if (strFieldType1  != strFieldType2)  
    { int a = 0; }
    if (strFieldValue1 != strFieldValue2) 
    { int a = 0; }
  }
}

void TestModelFields::CompareFieldMapStringMap(map<string,Field*> fieldMap,map<string,string> * stringMap)
{
  if (fieldMap.size() != (*stringMap).size())
  {
    throw new SystemException("Mapas não são do mesmo tamanho!","TestModelFields::CompareFieldMaps");
  }

  map<string,Field*>::iterator iterator1;
  for (iterator1 = fieldMap.begin(); iterator1 != fieldMap.end(); iterator1++)
  {
    string strFieldName = (*iterator1).first;
    Field * field1 = fieldMap[ strFieldName ];
    string strFieldValue1 = field1->ToString();
    string strFieldValue2 = (*stringMap)[ strFieldName ];
    if (strFieldValue1 != strFieldValue2) 
    { int a = 0; }
  }
}