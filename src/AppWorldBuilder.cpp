
#include "AppWorldBuilder.h"

//#include "XMLNodeFactory.h"
#include "StaticObstacle.h"
#include "Conversion.h"

///////////////////#include "Camera.h"
///////////////////#include "SensorCamera.h" 
#include "CanonVCC4.hh"
#include "SonyVID30.hh"
#include "MonoCam.h"
#include "ObserverCam.h"
#include "Box.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "SickLMS200.h"
///////////////////#include "Pioneer.h" 
#include "Pioneer2AT.hh"
#include "Pioneer2DX.hh"
#include "LightSource.h"
#include "GroundPlane.h"

////////////////////////////////////////////////////////
// Nao inclua nada do wxWidgets aqui senao vc tera
// problemas ao compilar com o Linux
////////////////////////////////////////////////////////


/*!
    \fn AppWorldBuilder::~AppWorldBuilder
    Destroi a aplicacao (model em MVC) WorldBuilder
 */
AppWorldBuilder::~AppWorldBuilder()
{
  
}

/*!
    \fn AppWorldBuilder::ReadWorldFile
    Carrega um arquivo World ( *.world ) para o objeto WorldFile.
    Pre-Condicao: WorldFile deve ter sido alocado previamente.
 */
void AppWorldBuilder::LoadWorldFile(const char * filename,WorldFile * worldfile)
{
  this->models.clear();

  throw new SystemException("AppWorldBuilder::LoadWorldFile","Metodo nao implementado !!");

  /*  
  worldfile->Load(filename);

  WorldFileNode * node = worldfile->GetRootNode();

  const char * name     = node->GetName();
  const char * nsprefix = node->GetNSPrefix();

  XMLNodeFactory xmlNodeFactory;
 
  for (node = node->GetChild(); node != NULL; node = node->GetNext())
  {
    if (node->GetNSPrefix())
    {
      string strPrefix = node->GetNSPrefix();

      // Check for model nodes
      if (strcmp( node->GetNSPrefix(), "model" ) == 0)
      {
        string strModelName = node->GetName();
        
        XMLModel * xmlModel = xmlNodeFactory.GetXMLNode(strModelName.c_str());
        
        if (xmlModel != NULL)
        {
          Model * model = xmlModel->Load(node);
          this->models.push_back(model);
          delete xmlModel;
        }      
      }

    }  
  }
  */
}

/*
    \fn AppWorldBuilder::NewWorldFile
*/
void AppWorldBuilder::NewWorldFile( const char * filename,WorldFile * world)
{
  StaticObstacle * model = new StaticObstacle();
  model->SetObstacleType("table");

  this->models.push_back(model);  
  
}

/*
    \fn AppWorldBuilder::SaveWorldFile
*/
void AppWorldBuilder::SaveWorldFile(const char * filename,WorldFile * world)
{
}

/*
    \fn AppWorldBuilder::GetModels
    Obtem os modelos 3D da classe de aplicacao (Model)
    para serem usados pela janela (View) 
*/
vector<Model *> * AppWorldBuilder::GetModels()
{
  return &this->models;
}





//void AppWorldBuilder::GetPropertyList(Model * model,map<string,string> * modelProperties)
//{
//  throw new SystemException("AppWorldBuilder::GetPropertyList","Nao implementando !!");
//
//  /*
//  //////////////////////////////////////////////////////////////////////////////////////
//  // Pega as propriedades do model
//  //////////////////////////////////////////////////////////////////////////////////////
//  (*modelProperties)["id"] = model->GetId();
//  (*modelProperties)["xyz"] = Conversion::GzVectorToString( model->GetPosition() );
//  (*modelProperties)["rpy"] = Conversion::GzVectorToString( model->GetOrientation() );
//  (*modelProperties)["gravity"] = Conversion::BoolToString( model->GetGravity() );
//  (*modelProperties)["enable"] = Conversion::BoolToString( model->GetEnable() );
//
//  //////////////////////////////////////////////////////////////////////////////////////
//  // Pega as propriedades herdadas da classe Camera
//  //////////////////////////////////////////////////////////////////////////////////////
//  if (dynamic_cast<Camera *>(model) != 0)
//  {
//    (*modelProperties)["updateRate"] = Conversion::DoubleToString( ((Camera *) model)->GetUpdateRate() );
//    (*modelProperties)["imageSize"]  = Conversion::IntegerToString( ((Camera *) model)->GetImageWidth()  ) + " " +
//                                       Conversion::IntegerToString( ((Camera *) model)->GetImageHeight() );
//    (*modelProperties)["hfov"] = Conversion::DoubleToString( ((Camera *) model)->GetHorizontalFieldOfView() );
//    (*modelProperties)["nearClip"] = Conversion::DoubleToString( ((Camera *) model)->GetNearClip() );
//    (*modelProperties)["farClip"]  = Conversion::DoubleToString( ((Camera *) model)->GetFarClip() );
//    (*modelProperties)["renderMethod"] = ((Camera *) model)->GetRenderMethod();
//    (*modelProperties)["zBufferDepth"] = Conversion::IntegerToString( ((Camera *) model)->GetZBufferDepth() );
//    (*modelProperties)["savePath"]   = ((Camera *) model)->GetSavePath();
//    (*modelProperties)["saveFrames"] = Conversion::BoolToString( ((Camera *) model)->GetSaveFrames() );
//
//    // Pegas as propriedades que herdam SensorCamera - CannonVCC4, SonyVID30
//    if (dynamic_cast<SensorCamera *>(model) != 0)
//    {
//      (*modelProperties)["userFrustum"] = Conversion::BoolToString( ((SensorCamera *) model)->GetUseFrustum() );
//      (*modelProperties)["zoomLimits"]  = Conversion::DoubleToString( ((SensorCamera *) model)->GetMinZoom() ) + " " +
//                                          Conversion::DoubleToString( ((SensorCamera *) model)->GetMaxZoom() );
//      (*modelProperties)["motionGain"]  = Conversion::DoubleToString( ((SensorCamera *) model)->GetMotionGain() );
//      (*modelProperties)["zoomGain"]    = Conversion::DoubleToString( ((SensorCamera *) model)->GetZoomGain() );
//    }
//    // Pega as propriedades que herdam de MonoCam
//    else if (dynamic_cast<MonoCam *>(model) != 0)
//    {
//      (*modelProperties)["useFrustum"] = Conversion::BoolToString( ((MonoCam *) model)->GetUseFrustum() );
//    }
//    // Pega as propriedades que herdam de ObserverCam
//    else if (dynamic_cast<ObserverCam *>(model) != 0)
//    {
//      (*modelProperties)["displayBBox"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplayBBox() );
//      (*modelProperties)["displayAxes"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplayAxes() );
//      (*modelProperties)["displayCoM"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplayCoM() );
//      (*modelProperties)["displayRays"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplayRays() );
//      (*modelProperties)["displayFrustums"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplayFrustums() );
//      (*modelProperties)["displayMaterials"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplayMaterials() );
//      (*modelProperties)["displayTextures"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplayTextures() );
//      (*modelProperties)["displaySkins"] = Conversion::BoolToString( ((ObserverCam *) model)->GetDisplaySkins() );
//      (*modelProperties)["polygonFill"] = Conversion::BoolToString( ((ObserverCam *) model)->GetPolygonFill() );
//      (*modelProperties)["shadeSmooth"] = Conversion::BoolToString( ((ObserverCam *) model)->GetShadeSmooth() );
//      (*modelProperties)["rollLock"] = Conversion::BoolToString( ((ObserverCam *) model)->GetRollLock() );
//    }
//  }    
//  /////////////////////////////////////////////////////////////////////////////////////
//  // Pega as propriedades herdadas da classe SimpleSolid
//  /////////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<SimpleSolid *>(model) != 0)
//  {
//    double c1, c2;
//    (*modelProperties)["shape"] = ((SimpleSolid *)model)->GetShape();
//    // Size ???
//    (*modelProperties)["mass"]  = Conversion::DoubleToString( ((SimpleSolid *)model)->GetMass() );
//    (*modelProperties)["color"] = Conversion::GzColorToString( ((SimpleSolid *)model)->GetColor() );
//    ((SimpleSolid *)model)->GetSurfaceHardness(c1,c2);
//    (*modelProperties)["surfaceHardness"] = Conversion::DoubleToString( c1 ) + " " + Conversion::DoubleToString( c2 );
//    ((SimpleSolid *)model)->GetSurfaceFriction(c1,c2);
//    (*modelProperties)["surfaceFriction"] = Conversion::DoubleToString( c1 ) + " " + Conversion::DoubleToString( c2 );
//    (*modelProperties)["transparent"] = Conversion::BoolToString( ((SimpleSolid *)model)->GetTransparency() );
//    (*modelProperties)["textureFile"] = ((SimpleSolid *)model)->GetTextureFile();
//    (*modelProperties)["skinFile"] = ((SimpleSolid *)model)->GetSkinFile();
//    (*modelProperties)["skinXyz"] = Conversion::GzVectorToString( ((SimpleSolid *)model)->GetSkinXYZ() );
//    (*modelProperties)["skinRpy"] = Conversion::GzVectorToString( ((SimpleSolid *)model)->GetSkinRPY() );
//    (*modelProperties)["skinScale"] = Conversion::GzVectorToString( ((SimpleSolid *)model)->GetSkinScale() );
//    (*modelProperties)["retro"] = Conversion::DoubleToString( ((SimpleSolid *)model)->GetRetro() );
//    (*modelProperties)["fiducial"] = Conversion::IntegerToString( ((SimpleSolid *)model)->GetFiducial() );
//
//    // Propriedade size de Box
//    if (dynamic_cast<Box *>(model) != 0)
//    {
//      (*modelProperties)["size"] = Conversion::GzVectorToString( ((Box*)model)->GetSize() );
//    }
//    // Propriedade size de Cylinder
//    else if (dynamic_cast<Cylinder *>(model) != 0)
//    {
//      (*modelProperties)["size"] = Conversion::DoubleToString( ((Cylinder*)model)->GetDiameter() ) + " " +
//                                   Conversion::DoubleToString( ((Cylinder*)model)->GetHeight() );
//    }
//    // Propriedade size de Sphere
//    else if (dynamic_cast<Sphere *>(model) != 0)
//    {
//      (*modelProperties)["size"] = Conversion::DoubleToString( ((Sphere*)model)->GetDiameter() );
//    }
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de Pioneer (Pioneer2AT e Pioneer2DX)
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<Pioneer *>(model) != 0)
//  {
//    (*modelProperties)["updateRate"]   = Conversion::DoubleToString( ((Pioneer*)model)->GetUpdateRate() );
//    (*modelProperties)["batteryLevel"] = Conversion::DoubleToString( ((Pioneer*)model)->GetBatteryLevel() );
//    double c1, c2;
//    ((Pioneer*)model)->GetBatteryCurve(c1,c2);
//    (*modelProperties)["batteryCurve"] = Conversion::DoubleToString( c1 ) + " " + Conversion::DoubleToString( c2 );
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de Pionner2Gripper
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<SickLMS200 *>(model) != 0)
//  {
//    (*modelProperties)["rangeCont"] = Conversion::IntegerToString( ((SickLMS200*)model)->GetRangeCount() );
//    (*modelProperties)["rayCount"]  = Conversion::IntegerToString( ((SickLMS200*)model)->GetRayCount() );
//    (*modelProperties)["rangeMax"]  = Conversion::DoubleToString( ((SickLMS200*)model)->GetMaxRange() );
//    (*modelProperties)["rangeMin"]  = Conversion::DoubleToString( ((SickLMS200*)model)->GetMinRange() );
//    (*modelProperties)["scanRate"]  = Conversion::DoubleToString( ((SickLMS200*)model)->GetScanRate() );
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de LightSource
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<LightSource *>(model) != 0)
//  {
//    (*modelProperties)["ambientColor"]  = Conversion::GzColorToString( ((LightSource*)model)->GetAmbientColor() );
//    (*modelProperties)["diffuseColor"]  = Conversion::GzColorToString( ((LightSource*)model)->GetDiffuseColor() );
//    (*modelProperties)["specularColor"] = Conversion::GzColorToString( ((LightSource*)model)->GetSpecularColor() );
//    (*modelProperties)["attenuation"]   = Conversion::GzVectorToString( ((LightSource*)model)->GetAttenuation() );
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de GroundPlane
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<GroundPlane *>(model) != 0)
//  {
//    (*modelProperties)["normal"] = Conversion::GzVectorToString( ((GroundPlane*)model)->GetNormal() );
//    (*modelProperties)["height"] = Conversion::DoubleToString( ((GroundPlane*)model)->GetHeight() );
//    (*modelProperties)["size"]   = Conversion::DoubleToString( ((GroundPlane*)model)->GetSizeX() ) + " " +
//                                   Conversion::DoubleToString( ((GroundPlane*)model)->GetSizeY() );
//  }
//  */
//}
//
//
//
//
//
//
//void AppWorldBuilder::SetPropertyList(Model * model,map<string,string> * modelProperties)
//{
//    throw new SystemException("AppWorldBuilder::GetPropertyList","Nao implementando !!");
//
//  /*
//  //////////////////////////////////////////////////////////////////////////////////////
//  // Pega as propriedades do model
//  //////////////////////////////////////////////////////////////////////////////////////
//  model->SetId( (*modelProperties)["id"] );
//  model->SetPosition( Conversion::StringToGzVector( (*modelProperties)["xyz"] ) );
//  model->SetOrientation( Conversion::StringToGzVector( (*modelProperties)["rpy"] ) );
//  model->SetGravity( Conversion::StringToBool( (*modelProperties)["gravity"] ) );
//  model->SetEnable( Conversion::StringToBool( (*modelProperties)["enable"] ) );
//
//  //////////////////////////////////////////////////////////////////////////////////////
//  // Pega as propriedades herdadas da classe Camera
//  //////////////////////////////////////////////////////////////////////////////////////
//  if (dynamic_cast<Camera *>(model) != 0)
//  {
//    int c1, c2;
//    ((Camera*)model)->SetUpdateRate( Conversion::StringToDouble((*modelProperties)["updateRate"]) );
//    Conversion::StringToIntegerTuple( (*modelProperties)["size"],c1,c2 );
//    ((Camera*)model)->SetImageWidth(c1);
//    ((Camera*)model)->SetImageHeight(c2);
//    ((Camera*)model)->SetHorizontalFieldOfView( Conversion::StringToDouble( (*modelProperties)["hfov"] ) );
//    ((Camera*)model)->SetNearClip(  Conversion::StringToDouble( (*modelProperties)["nearClip"] ) );
//    ((Camera*)model)->SetFarClip(   Conversion::StringToDouble( (*modelProperties)["farClip"] ) );
//    ((Camera*)model)->SetRenderMethod( (*modelProperties)["renderMethod"] );
//    ((Camera*)model)->SetZBufferDepth( Conversion::StringToInt( (*modelProperties)["zBufferDepth"] ) );
//    ((Camera*)model)->SetSavePath( (*modelProperties)["savePath"] );
//    ((Camera*)model)->SetSaveFrames( Conversion::StringToBool( (*modelProperties)["saveFrames"] ) );    
//
//    // Pegas as propriedades que herdam SensorCamera - CannonVCC4, SonyVID30
//    if (dynamic_cast<SensorCamera *>(model) != 0)
//    {
//      ((SensorCamera*)model)->SetUseFrustum( Conversion::StringToBool( (*modelProperties)["useFrustum"] ) );
//      double c1,c2;
//      Conversion::StringToDoubleTuple((*modelProperties)["zoomLimits"], c1, c2);
//      ((SensorCamera*)model)->SetMinZoom( c1 );
//      ((SensorCamera*)model)->SetMaxZoom( c2 );
//      ((SensorCamera*)model)->SetMotionGain( Conversion::StringToDouble( (*modelProperties)["motionGain"] ) );
//      ((SensorCamera*)model)->SetZoomGain( Conversion::StringToDouble( (*modelProperties)["zoomGain"] ) );
//    }
//    // Pega as propriedades que herdam de MonoCam
//    else if (dynamic_cast<MonoCam *>(model) != 0)
//    {
//      ((MonoCam*) model)->SetUseFrustum( Conversion::StringToBool( (*modelProperties)["useFrustum"] ) );
//    }
//    // Pega as propriedades que herdam de ObserverCam
//    else if (dynamic_cast<ObserverCam *>(model) != 0)
//    {
//      ((ObserverCam*)model)->SetDisplayBBox( Conversion::StringToBool( (*modelProperties)["displayBBox"] ) );
//      ((ObserverCam*)model)->SetDisplayAxes( Conversion::StringToBool( (*modelProperties)["displayAxes"] ) );
//      ((ObserverCam*)model)->SetDisplayCoM ( Conversion::StringToBool( (*modelProperties)["displayCoM"] ) );
//      ((ObserverCam*)model)->SetDisplayRays( Conversion::StringToBool( (*modelProperties)["displayRays"] ) );
//      ((ObserverCam*)model)->SetDisplayFrustums( Conversion::StringToBool( (*modelProperties)["displayFrustums"] ) );
//      ((ObserverCam*)model)->SetDisplayMaterials( Conversion::StringToBool( (*modelProperties)["displayMaterials"] ) );
//      ((ObserverCam*)model)->SetDisplayTextures( Conversion::StringToBool( (*modelProperties)["displayTextures"] ) );
//      ((ObserverCam*)model)->SetDisplaySkins( Conversion::StringToBool( (*modelProperties)["displaySkins"] ) );
//      ((ObserverCam*)model)->SetPolygonFill( Conversion::StringToBool( (*modelProperties)["polygonFill"] ) );
//      ((ObserverCam*)model)->SetShadeSmooth( Conversion::StringToBool( (*modelProperties)["shadeSmooth"] ) );
//      ((ObserverCam*)model)->SetRollLock( Conversion::StringToBool( (*modelProperties)["rollLock"] ) ); 
//    }
//  }    
//  /////////////////////////////////////////////////////////////////////////////////////
//  // Pega as propriedades herdadas da classe SimpleSolid
//  /////////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<SimpleSolid *>(model) != 0)
//  {
//    // Shape - Ja é setado antes
//    ((SimpleSolid*)model)->SetMass(  Conversion::StringToDouble(  (*modelProperties)["mass"] ) );
//    ((SimpleSolid*)model)->SetColor( Conversion::StringToGzColor( (*modelProperties)["color"] ) );
//    double c1, c2;
//    Conversion::StringToDoubleTuple( (*modelProperties)["surfaceHardness"], c1, c2);
//    ((SimpleSolid*)model)->SetSurfaceHardness(c1,c2);
//    Conversion::StringToDoubleTuple( (*modelProperties)["surfaceFriction"], c1, c2);
//    ((SimpleSolid*)model)->SetSurfaceFriction(c1,c2);
//    ((SimpleSolid*)model)->SetTransparency( Conversion::StringToBool( (*modelProperties)["transparent"] ) );
//    ((SimpleSolid*)model)->SetTextureFile( (*modelProperties)["textureFile"] );
//    ((SimpleSolid*)model)->SetSkinFile( (*modelProperties)["skinFile"] );
//    ((SimpleSolid*)model)->SetSkinXYZ( Conversion::StringToGzVector( (*modelProperties)["skinXyz"] ) );
//    ((SimpleSolid*)model)->SetSkinRPY( Conversion::StringToGzVector( (*modelProperties)["skinRpy"] ) );
//    ((SimpleSolid*)model)->SetSkinScale( Conversion::StringToGzVector( (*modelProperties)["skinScale"] ) );
//    ((SimpleSolid*)model)->SetRetro( Conversion::StringToDouble( (*modelProperties)["retro"] ) );
//    ((SimpleSolid*)model)->SetFiducial( Conversion::StringToInt( (*modelProperties)["fiducial"] ) );
//
//    // Propriedade size de Box
//    if (dynamic_cast<Box *>(model) != 0)
//    {
//      ((Box*)model)->SetSize( Conversion::StringToGzVector( (*modelProperties)["size"] ) );
//    }
//    // Propriedade size de Cylinder
//    else if (dynamic_cast<Cylinder *>(model) != 0)
//    {
//      double diameter, height;
//      Conversion::StringToDoubleTuple((*modelProperties)["size"],diameter,height);
//      ((Cylinder*)model)->SetDiameter(diameter);
//      ((Cylinder*)model)->SetHeight(height);
//    }
//    // Propriedade size de Sphere
//    else if (dynamic_cast<Sphere *>(model) != 0)
//    {
//      ((Sphere*)model)->SetDiameter( Conversion::StringToDouble( (*modelProperties)["size"] ) );
//    }
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de Pioneer (Pioneer2AT e Pioneer2DX)
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<Pioneer *>(model) != 0)
//  {
//    ((Pioneer*)model)->SetUpdateRate( Conversion::StringToDouble( (*modelProperties)["updateRate"] ) );
//    ((Pioneer*)model)->SetBatteryLevel( Conversion::StringToDouble( (*modelProperties)["batteryLevel"] ) );
//    double c1, c2;
//    Conversion::StringToDoubleTuple( (*modelProperties)["batteryCurve"], c1, c2);
//    ((Pioneer*)model)->SetBatteryCurve(c1,c2);
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de Pionner2Gripper
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<SickLMS200 *>(model) != 0)
//  {
//    ((SickLMS200*)model)->SetRangeCount( Conversion::StringToInt( (*modelProperties)["rangeCont"] ) );
//    ((SickLMS200*)model)->SetRayCount( Conversion::StringToInt( (*modelProperties)["rayCount"] ) );
//    ((SickLMS200*)model)->SetMaxRange( Conversion::StringToDouble( (*modelProperties)["rangeMax"] ) );
//    ((SickLMS200*)model)->SetMinRange( Conversion::StringToDouble( (*modelProperties)["rangeMin"] ) );
//    ((SickLMS200*)model)->SetScanRate( Conversion::StringToDouble( (*modelProperties)["scanRate"] ) );
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de LightSource
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<LightSource *>(model) != 0)
//  {
//    ((LightSource*)model)->SetAmbientColor( Conversion::StringToGzColor( (*modelProperties)["ambientColor"] ) );
//    ((LightSource*)model)->SetDiffuseColor( Conversion::StringToGzColor( (*modelProperties)["diffuseColor"] ) );
//    ((LightSource*)model)->SetSpecularColor(Conversion::StringToGzColor( (*modelProperties)["specularColor"] ) );
//    ((LightSource*)model)->SetAttenuation(  Conversion::StringToGzVector((*modelProperties)["attenuation"] ) );
//  }
//  //////////////////////////////////////////////////////////////////////////////////
//  // Propriedades de GroundPlane
//  //////////////////////////////////////////////////////////////////////////////////
//  else if (dynamic_cast<GroundPlane *>(model) != 0)
//  {
//    ((GroundPlane*)model)->SetNormal( Conversion::StringToGzVector( (*modelProperties)["normal"] ) );
//    ((GroundPlane*)model)->SetHeight( Conversion::StringToDouble( (*modelProperties)["height"] ) );
//    double sizeX,sizeY;
//    Conversion::StringToDoubleTuple((*modelProperties)["size"],sizeX,sizeY);
//    ((GroundPlane*)model)->SetSizeX(sizeX);
//    ((GroundPlane*)model)->SetSizeY(sizeY);
//  }
//  */
//}

