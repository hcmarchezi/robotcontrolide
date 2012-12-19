
#ifndef XMLCOMMAND_H
#define XMLCOMMAND_H

#include "Model.hh"
#include "tinyxml.h"
#include "World.hh"

class XMLCommand
{
public: static TiXmlElement * Serialize(Model * model);
public: static Model * Deserialize(TiXmlElement * modelElement,VirtualWorld * virtualWorld );
};

#endif

