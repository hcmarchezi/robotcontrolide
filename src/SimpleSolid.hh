/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003  
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: Model for simple solids
 * Author: Andrew Howard
 * Date: 3 Aug 2003
 * CVS: $Id: SimpleSolid.hh,v 1.5 2004/05/24 05:49:25 inspectorg Exp $
 */

#ifndef SIMPLESOLID_HH
#define SIMPLESOLID_HH

#include "Body.hh"
#include "Model.hh"

// Forward declarations
class Geom;


class SimpleSolid : public Model
{
  public: SimpleSolid( World *world,const char * shape);

  // Constructor, destructor
  public: SimpleSolid( World *world );
  public: virtual ~SimpleSolid();
  
  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );

  // Load ODE stuff
  private: int OdeLoad( WorldFile *file, WorldFileNode *node );

  // ODE objects
  private: Body *body;
  private: Geom *geom;

  //////////////////////////////////////////////////////////////////
  // Inicia a geometria a as propriedades do modelo
  //////////////////////////////////////////////////////////////////
  public: void Start();

  //////////////////////////////////////////////////////////////////
  // Metodos get/set para manipular propriedades com meta-dados
  // utilizadas pelo IRCE
  //////////////////////////////////////////////////////////////////

private: string shape;
public: void SetShape(string shape);
public: string GetShape();

public: GzColor GetColor();
public: void SetColor(GzColor color);


// Atributo auxiliar que guarda um numero double para servir como referenciar
// ao retornar o tamanho do SimpleSolid quando este for uma sphere
private: double sphereSize;
private: GzVector boxSize;
private: DoubleTuple cylindersize;
// size
// O tipo de dado de size é dinâmico e varia de acordo com o 
// shape escolhido
// Se box entao tipo de dado e GzVector
// Se cylinder entao tipo de dado e DoubleTuple
// Se sphere entao tipo de dados e Double
public: void SetSize(const void* size);
public: const void* GetSize();

  //////////////////////////////////////////////////////////////////


};


#endif
