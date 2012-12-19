
#include "SensorCamera.h"

SensorCamera::SensorCamera(void)
{
  this->SetField(new DoubleField("updateRate",0.0));
  this->SetField(new IntegerTupleField("imageSize",IntegerTupleSet(320,240)));
  this->SetField(new DoubleField("nearClip",0.0));
  this->SetField(new DoubleField("farClip",0.0));
  this->SetField(new StringField("renderMethod",""));
  this->SetField(new IntegerField("zBufferDepth",0));
  this->SetField(new StringField("savePath",""));
  this->SetField(new BoolField("saveFrames",false));

  this->SetField(new BoolField("useFrustum",false));
  this->SetField(new DoubleField("minZoom",0.0));
  this->SetField(new DoubleField("maxZoom",0.0));
  this->SetField(new DoubleField("motionGain",0.0));
  this->SetField(new DoubleField("zoomGain",0.0)); 
}

SensorCamera::~SensorCamera(void)
{
}



// Update Period Rate
double SensorCamera::GetUpdateRate()
{
  return ((DoubleField*)this->GetField("updateRate"))->GetValue();
}
void SensorCamera::SetUpdateRate(double updateRate)
{
  ((DoubleField*)this->GetField("updateRate"))->SetValue(updateRate);
}


// Image Size
IntegerTuple & SensorCamera::GetImageSize()
{
  return ((IntegerTupleField*)this->GetField("imageSize"))->GetValue();
}
void SensorCamera::SetImageSize(IntegerTuple imageSize)
{
  ((IntegerTupleField*)this->GetField("imageSize"))->SetValue(imageSize);
}

// HFOV
double SensorCamera::GetHorizontalFieldOfView()
{
  return ((DoubleField*)this->GetField("horizontalFieldOfView"))->GetValue();
}
void SensorCamera::SetHorizontalFieldOfView(double horizontalFieldOfView)
{
	((DoubleField*)this->GetField("horizontalFieldOfView"))->SetValue(horizontalFieldOfView);
}

// Near Clip
double SensorCamera::GetNearClip()
{
  return ((DoubleField*)this->GetField("nearClip"))->GetValue();
}
void   SensorCamera::SetNearClip(double nearClip)
{
	((DoubleField*)this->GetField("nearClip"))->SetValue(nearClip);
}

// Far Clip
double SensorCamera::GetFarClip()
{
  return ((DoubleField*)this->GetField("farClip"))->GetValue();
}
void   SensorCamera::SetFarClip(double farClip)
{
	((DoubleField*)this->GetField("farClip"))->SetValue(farClip);
}

// Render Method
string SensorCamera::GetRenderMethod()
{
  return ((StringField*)this->GetField("renderMethod"))->GetValue();
}
void   SensorCamera::SetRenderMethod(string renderMethod)
{
	((StringField*)this->GetField("renderMethod"))->SetValue(renderMethod);
}

// Z Buffer Depth
int  SensorCamera::GetZBufferDepth()
{
	return ((IntegerField*)this->GetField("zBufferDepth"))->GetValue();
}
void SensorCamera::SetZBufferDepth(int zBufferDepth)
{
	if (( zBufferDepth != 8  )||( zBufferDepth != 16 )||
		( zBufferDepth != 24 )||( zBufferDepth != 32 ))
	{
		((IntegerField*)this->GetField("zBufferDepth"))->SetValue(zBufferDepth);
	}
  else
  {
    throw new SystemException("SensorCamera::SetZBufferDepth","O valor do zBuffer informado e invalido. O valor permitidos: 8,16,24,32 ");
  }
}

// Save Path
string SensorCamera::GetSavePath()
{
  return ((StringField*)this->GetField("savePath"))->GetValue();  
}
void SensorCamera::SetSavePath(string savePath)
{
  ((StringField*)this->GetField("savePath"))->SetValue(savePath);
}

// Save Frames
bool SensorCamera::GetSaveFrames()
{
	return ((BoolField*)this->GetField("saveFrames"))->GetValue();  
}
void SensorCamera::SetSaveFrames(bool saveFrames)
{
  ((BoolField*)this->GetField("saveFrames"))->SetValue(saveFrames);
}



// Use Frustum
bool SensorCamera::GetUseFrustum()
{
	return ((BoolField*)this->GetField("useFrustum"))->GetValue();  
}
void SensorCamera::SetUseFrustum(bool useFrustum)
{
  ((BoolField*)this->GetField("useFrustum"))->SetValue(useFrustum);
}

// Min Zoom Limit
double SensorCamera::GetMinZoom()
{
  return ((DoubleField*)this->GetField("minZoom"))->GetValue();
}
void SensorCamera::SetMinZoom(double minZoom)
{
	((DoubleField*)this->GetField("minZoom"))->SetValue(minZoom);
}

// Max Zoom Limit
double SensorCamera::GetMaxZoom()
{
  return ((DoubleField*)this->GetField("maxZoom"))->GetValue();
}
void SensorCamera::SetMaxZoom(double maxZoom)
{
	((DoubleField*)this->GetField("maxZoom"))->SetValue(maxZoom);
}

// Motion Gain
double SensorCamera::GetMotionGain()
{
  return ((DoubleField*)this->GetField("motionGain"))->GetValue();
}
void SensorCamera::SetMotionGain(double motionGain)
{
  ((DoubleField*)this->GetField("motionGain"))->SetValue(motionGain);
}

// Zoom Gain
double SensorCamera::GetZoomGain()
{
  return ((DoubleField*)this->GetField("zoomGain"))->GetValue();
}
void SensorCamera::SetZoomGain(double zoomGain)
{
	((DoubleField*)this->GetField("zoomGain"))->SetValue(zoomGain);
}

