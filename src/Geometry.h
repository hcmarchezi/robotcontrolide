
#ifndef GEOMETRY_H
#define GEOMETRY_H

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Color.hh"
#include "Vector.hh"

#include "RenderOptions.hh"


class Geometry
{
public: static void SetColor(GzColor color);
public: static void DrawBox(RenderOptions *renderOptions, GLdouble sizeX,GLdouble sizeY,GLdouble sizeZ);
public: static void DrawCircle(RenderOptions *renderOptions,GLdouble radius);
public: static void DrawWheel(RenderOptions *renderOptions,GLdouble wheelRadius,GLdouble wheelThick);
public: static void DrawSphere(RenderOptions *renderOptions,GLdouble radius);
public: static void DrawCylinder(RenderOptions *renderOptions,GLdouble baseRadius,GLdouble topRadius,GLdouble height);
public: static void DrawPrism(RenderOptions *renderOptions,int concave, double extractVector[3], int nbVertices, double *baseVertices);
public: static void DrawPlaneGround(RenderOptions *renderOptions,double altitude,GzVector normal,const char * textureFile,GzColor color);
};

#endif

