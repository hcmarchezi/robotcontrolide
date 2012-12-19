
#include "Geometry.h"

///////////////////////////////////////#include "ViewOptions.h"
#include "RenderOptionsSingleton.h"
#include <iostream>
#include <cmath>


void Geometry::DrawBox(RenderOptions *renderOptions,GLdouble sizeX,GLdouble sizeY,GLdouble sizeZ)
{
  GLdouble px = sizeX / 2.0;
  GLdouble py = sizeY / 2.0;
  GLdouble pz = sizeZ / 2.0;
  
  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)    //ViewOptions::GetInstance()->mode == SolidMesh)
  {
    glBegin(GL_QUADS);
  }
  else
  {
    glBegin(GL_LINES);
  }
  
  glNormal3d(0.0,0.0,1.0);
  glVertex3d(-px,-py, pz);
  glVertex3d( px,-py, pz);
  glVertex3d( px, py, pz);
  glVertex3d(-px, py, pz);

  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_LINE)  ///ViewOptions::GetInstance()->mode == WireMesh)
  {
    glVertex3d(-px,-py, pz);
    glVertex3d(-px, py, pz);
    glVertex3d( px,-py, pz);
    glVertex3d( px, py, pz);   
  }
 
  glNormal3d(0.0,0.0,-1.0);
  glVertex3d(-px,-py,-pz);
  glVertex3d( px,-py,-pz);
  glVertex3d( px, py,-pz);
  glVertex3d(-px, py,-pz);

  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_LINE)  //ViewOptions::GetInstance()->mode == WireMesh)
  {
    glVertex3d(-px,-py,-pz);
    glVertex3d(-px, py,-pz);
    glVertex3d( px,-py,-pz);
    glVertex3d( px, py,-pz);   
  }
 
  glNormal3d(0.0,1.0,0.0);
  glVertex3d(-px, py,-pz);
  glVertex3d( px, py,-pz);
  glVertex3d( px, py, pz);
  glVertex3d(-px, py, pz);

  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_LINE)  //ViewOptions::GetInstance()->mode == WireMesh)
  {
    glVertex3d(-px, py,-pz);
    glVertex3d(-px, py, pz);
    glVertex3d( px, py,-pz);
    glVertex3d( px, py, pz);
  }

  glNormal3d(0.0,-1.0,0.0);
  glVertex3d(-px,-py,-pz);
  glVertex3d( px,-py,-pz);
  glVertex3d( px,-py, pz);
  glVertex3d(-px,-py, pz);

  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_LINE) //ViewOptions::GetInstance()->mode == WireMesh)
  {
    glVertex3d(-px,-py,-pz);
    glVertex3d(-px,-py, pz);
    glVertex3d( px,-py,-pz);
    glVertex3d( px,-py, pz);
  }

  glNormal3d(-1.0,0.0,0.0);
  glVertex3d(-px,-py,-pz);
  glVertex3d(-px, py,-pz);
  glVertex3d(-px, py, pz);
  glVertex3d(-px,-py, pz);

  glNormal3d(1.0,0.0,0.0);
  glVertex3d( px,-py,-pz);
  glVertex3d( px, py,-pz);
  glVertex3d( px, py, pz);
  glVertex3d( px,-py, pz);

  glEnd();
}

void Geometry::DrawCircle(RenderOptions *renderOptions,GLdouble radius)
{
  GLUquadricObj * quadric = gluNewQuadric();

  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)  //ViewOptions::GetInstance()->mode == SolidMesh)
  {
    gluQuadricDrawStyle(quadric, GLU_FILL);  
  }
  else
  {
    gluQuadricDrawStyle(quadric, GLU_LINE);
  }
  glColor3d(0.0,0.0,1.0);
  gluCylinder(quadric,0.0,radius,0.001,20,1);
}

void Geometry::DrawWheel(RenderOptions *renderOptions,GLdouble wheelRadius,GLdouble wheelThick)
{  
  GLUquadricObj * quadric = gluNewQuadric();

  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL) //ViewOptions::GetInstance()->mode == SolidMesh)
  {
    gluQuadricDrawStyle(quadric, GLU_FILL);  
  }
  else
  {
    gluQuadricDrawStyle(quadric, GLU_LINE);
  }
  glPushMatrix();
  glTranslated(0.0,0.0,0.0);
  gluCylinder(quadric,0.0,wheelRadius,0.001,20,1);
  glPopMatrix();
  gluCylinder(quadric,wheelRadius,wheelRadius,wheelThick,20,2);
  glPushMatrix();
  glTranslated(0.0,0.0,wheelThick);
  glRotated(180,1.0,0.0,0.0);
  gluCylinder(quadric,0.0,wheelRadius,0.001,20,1);
  glPopMatrix();
}

void Geometry::DrawSphere(RenderOptions *renderOptions,GLdouble radius)
{
  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)//(ViewOptions::GetInstance()->mode == SolidMesh)
  {
    glutSolidSphere(radius,40,40);
  }
  else
  {
    glutWireSphere(radius,10,10);
  }
}

void Geometry::SetColor(GzColor color)
{
  GLfloat mat_ambient[] = {color.r*.2, color.g*.2, color.b*.2, color.a*.2};
  GLfloat mat_diffuse[] = {color.r,    color.g,    color.b,    color.a};
  GLfloat mat_specular[]= {color.r*1.2,color.g*1.2,color.b*1.2,color.a*1.2};
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
}

void Geometry::DrawCylinder(RenderOptions *renderOptions,GLdouble baseRadius,GLdouble topRadius,GLdouble height)
{
  GLUquadricObj * quadric = gluNewQuadric();

  glPushMatrix();

  glTranslated(0.0,0.0,-height/2.0);      
  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)//(ViewOptions::GetInstance()->mode == SolidMesh)
  {
    gluQuadricDrawStyle(quadric, GLU_FILL);  
  }
  else
  {
    gluQuadricDrawStyle(quadric, GLU_LINE);
  }
  gluCylinder(quadric,baseRadius,topRadius,height,20,5);
  
  gluCylinder(quadric,0.0,baseRadius,0.0,20,5);
  
  glPushMatrix();
  glTranslated(0.0,0.0,height);
  glRotated(180.0,1.0,0.0,0.0);
  gluCylinder(quadric,0.0,topRadius,0.0,20,5);
  glPopMatrix();

  glPopMatrix();
}

void Geometry::DrawPrism(RenderOptions *renderOptions,int concave, double extractVector[3], int nbVertices, double *baseVertices)
{    
  // Initialize vertices
  double** vertices0 = new double*[nbVertices];
  double** vertices1 = new double*[nbVertices];
  double** norm = new double*[nbVertices];
  for(int i=0; i<nbVertices; i++)
  {
    vertices0[i] = new double[3];
    vertices1[i] = new double[3];
    norm[i] = new double[3];
  }
  for(int i=0; i<nbVertices; i++)
  {
    for(int j=0; j<3; j++)
    {
      vertices0[i][j] = baseVertices[3*i+j];
      vertices1[i][j] = vertices0[i][j] + extractVector[j];
    }
  }

  // Compute normal vectors
  double a[3], b[3];
  double c;
  int n = nbVertices;
  double norm0[3], norm1[3];

  c = sqrt( extractVector[0]*extractVector[0] 
      + extractVector[1]*extractVector[1] 
      + extractVector[2]*extractVector[2] );

  norm1[0] = extractVector[0]/c;
  norm1[1] = extractVector[1]/c;
  norm1[2] = extractVector[2]/c;

  norm0[0] = -norm1[0];
  norm0[1] = -norm1[1];
  norm0[2] = -norm1[2];

  for(int i=0; i<nbVertices-1; i++)
  {
    for(int j=0; j<3; j++)
    {
      a[j] = vertices0[i+1][j] - vertices0[i][j];
    }
    b[0] = a[1]*extractVector[2] - extractVector[1]*a[2];
    b[1] = a[2]*extractVector[0] - extractVector[2]*a[0];
    b[2] = a[0]*extractVector[1] - extractVector[0]*a[1];
    c = sqrt( b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);
    norm[i][0] = b[0]/c;
    norm[i][1] = b[1]/c;
    norm[i][2] = b[2]/c;
  }

  a[0] = vertices0[0][0] - vertices0[n-1][0];
  a[1] = vertices0[0][1] - vertices0[n-1][1];
  a[2] = vertices0[0][2] - vertices0[n-1][2];
  b[0] = a[1]*extractVector[2] - extractVector[1]*a[2];
  b[1] = a[2]*extractVector[0] - extractVector[2]*a[0];
  b[2] = a[0]*extractVector[1] - extractVector[0]*a[1];
  c = sqrt( b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);
  norm[n-1][0] = b[0]/c;
  norm[n-1][1] = b[1]/c;
  norm[n-1][2] = b[2]/c;

  int i;
  GLUtriangulatorObj *tobj = NULL;
  n = nbVertices;
  
  // GL rendering
  if(concave)
  {
    // define a new tesselation object
    if(tobj == NULL)
    {
      tobj = gluNewTess();
      if(tobj == NULL)
      {
        std::cerr << "pb newTess\n";
      }

      #ifndef WIN32
      gluTessCallback(tobj, GLU_TESS_BEGIN,(GLvoid (*) ()) &glBegin);
      gluTessCallback(tobj, GLU_TESS_VERTEX, (GLvoid (*) ()) &glVertex3dv);
      #else
      gluTessCallback(tobj, GLU_TESS_BEGIN,(GLvoid (__stdcall *)())&glBegin);
      gluTessCallback(tobj, GLU_TESS_VERTEX,(GLvoid (__stdcall *)())&glVertex3dv);
      #endif

      gluTessCallback(tobj, GLU_TESS_END, glEnd);
    }
    // first face
    gluBeginPolygon(tobj);
    glNormal3dv(norm0);
    for(i=0; i<n; i++)
    {
      gluTessVertex(tobj, vertices0[n-i-1], vertices0[n-i-1]);
    }
    gluEndPolygon(tobj);
    // second face
    gluBeginPolygon(tobj);
    glNormal3dv(norm1);
    for(i=0; i<n; i++)
    {
      gluTessVertex(tobj, vertices1[i], vertices1[i]);
    }
    gluEndPolygon(tobj);
  }
  else
  {
    // convex polygon

    // first face
    if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)//(ViewOptions::GetInstance()->mode == SolidMesh)
    {
      glBegin(GL_POLYGON);
    }
    else
    {
      glBegin(GL_LINES);
    }
    glNormal3dv(norm0);
    for(i=0; i<n; i++)
    {
      glVertex3dv(vertices0[n-i-1]);
    }
    glEnd();
    // seconde face
    if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)//(ViewOptions::GetInstance()->mode == SolidMesh)
    {
      glBegin(GL_POLYGON);
    }
    else
    {
      glBegin(GL_LINES);
    }
    glNormal3dv(norm1);
    for(i=0; i<n; i++)
    {
      glVertex3dv(vertices1[i]);
    }
    glEnd();

  }

  // sides
  glShadeModel(GL_FLAT);
  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)//(ViewOptions::GetInstance()->mode == SolidMesh)
  {
    glBegin(GL_QUAD_STRIP);
  }
  else
  {
    glBegin(GL_LINES);
  }  
  glNormal3dv(norm[0]);
  glVertex3dv(vertices1[0]);
  glVertex3dv(vertices0[0]);
  for(i=0; i<n; i++)
  {
    glVertex3dv(vertices1[i]);
    glVertex3dv(vertices0[i]);
    glNormal3dv(norm[i]);
  }
  glVertex3dv(vertices1[0]);
  glVertex3dv(vertices0[0]);
  glEnd();
  glShadeModel(GL_SMOOTH);

  delete vertices0;
  delete vertices1;
  delete norm;
}

void Geometry::DrawPlaneGround(RenderOptions *renderOptions,double altitude,GzVector normal,const char * textureFile,GzColor color)
{
  // Iniciando variaveis de configuracao
  GzColor colorAmbient  = GzColor( color.r*0.2,color.g*0.2,color.b*0.2,color.a*1.0 );
  GzColor colorDiffuse  = GzColor( color.r*0.8,color.g*0.8,color.b*0.8,color.a*1.0 );
  GzColor colorSpecular = GzColor( color.r*0.2,color.g*0.2,color.b*0.2,color.a*1.0 );
  double shininess = 10.0;

  // Se estiver em modo sem sombra atualiza o glColor
  glColor3f( color.r, color.g, color.b );

  //// Transforma a normal informada num vetor unitário
  double modulo_normal = sqrt(pow(normal.x,2) + pow(normal.y,2) + pow(normal.z,2) );
  normal.x = normal.x/modulo_normal;
  normal.y = normal.y/modulo_normal;
  normal.z = normal.z/modulo_normal;
  
  // Set material properties
  if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)//(ViewOptions::GetInstance()->mode == SolidMesh)
  {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colorAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colorDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colorSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    //PRINT_GL_ERR();
  }

  //if ((ViewOptions::GetInstance()->mode == SolidMesh)&&(textureFile != NULL))
  //{
  //  // Set up textures
  //  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //  
  //  // Build the mipmaps
  //  // Assume image data is *not* aligned
  //  glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
  //  gluBuild2DMipmaps(GL_TEXTURE_2D, this->textureImage->components,
  //                    this->textureImage->width, this->textureImage->height,
  //                    this->textureImage->format, this->textureImage->type,
  //                    this->textureImage->data);  

  //  glEnable(GL_TEXTURE_2D);
  //}

  glNormal3f(normal.x, normal.y, normal.z);
  
  // Compute number of patches
  double patchSize = 8.0;
  int ni = 10; //(int) (ceil(opt->farClip / this->patchSize));
  int nj = 10; //(int) (ceil(opt->farClip / this->patchSize));
  double scale = 5.0;
    
  GzVector p, q, offset;

  // Render strips
  for (int j = -nj; j < nj; j += 1)
  {
    if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_FILL)//( ViewOptions::GetInstance()->mode == SolidMesh )
    {
      glBegin(GL_TRIANGLE_STRIP);
      
      for (int i = -ni; i < ni; i += 2)
      {
        double vertex[4][2] = {{-1, +1}, {-1, -1}, {+1, +1}, {+1, -1}};
        //offset = GzVectorSet(i * this->patchSize, j * this->patchSize, 0.0);
          
        for (int k = 0; k < 4; k++)
        {
          q = GzVectorSet(vertex[k][0] * patchSize, vertex[k][1] * patchSize, 0.0);
          
          //q = GzVectorMul(0.5, q);
          //q = GzVectorAdd(q, offset);

          //p = GzVectorMul(q.x, this->planeNx);
          //p = GzVectorAdd(p, GzVectorMul(q.y, this->planeNy));

          // Texture is scaled so that it covers the right amount of the plane
          //glTexCoord2f(q.x  / this->textureSize.x, q.y / this->textureSize.y);
          
          //glVertex3f(p.x, p.y, p.z);

          glVertex3f(q.x,q.y,q.z);
        }
      }
      glEnd();
    }
    else
    {
      glDisable(GL_LIGHTING);

      glColor4f(color.r,color.g,color.b,color.a);

      double vertex[5][2] = {{-1, 1},{-1,-1},{ 1,-1},{ 1, 1},{-1, 1}};
           
      for (double i = -patchSize/2.0; i <= patchSize/2.0; i ++)
      {
        for (double j = -patchSize/2.0; j <= patchSize/2.0; j++)
        { 
          glBegin(GL_LINE_STRIP);  

          //offset = GzVectorSet(i * this->patchSize, j * this->patchSize, 0.0);            
          for (int k = 0; k < 5; k++)
          {
            glVertex3f((vertex[k][0]+i)*scale, (vertex[k][1]+j)*scale, 0.0);
          }

          glEnd();
        }
      }      
    
      glEnable(GL_LIGHTING);
    }
  }

  //if ((ViewOptions::GetInstance()->mode == SolidMesh)&&(textureFile != NULL))
  //{
  //  glDisable(GL_TEXTURE_2D);
  //}
}


