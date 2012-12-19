
#include "Camera.h"

#include "SystemException.h"

Camera::Camera(void)
{
  this->SetField(new DoubleField("updateRate",0.0));
  this->SetField(new IntegerTupleField("imageSize",IntegerTupleSet(320,240)));
  this->SetField(new DoubleField("nearClip",0.0));
  this->SetField(new DoubleField("farClip",0.0));
  this->SetField(new StringField("renderMethod",""));
  this->SetField(new IntegerField("zBufferDepth",0));
  this->SetField(new StringField("savePath",""));
  this->SetField(new BoolField("saveFrames",false));
}

Camera::~Camera(void)
{
}

// Update Period Rate
double Camera::GetUpdateRate()
{
  return ((DoubleField*)this->GetField("updateRate"))->GetValue();
}
void Camera::SetUpdateRate(double updateRate)
{
  ((DoubleField*)this->GetField("updateRate"))->SetValue(updateRate);
}


// Image Size
IntegerTuple & Camera::GetImageSize()
{
  return ((IntegerTupleField*)this->GetField("imageSize"))->GetValue();
}
void Camera::SetImageSize(IntegerTuple imageSize)
{
  ((IntegerTupleField*)this->GetField("imageSize"))->SetValue(imageSize);
}

// HFOV
double Camera::GetHorizontalFieldOfView()
{
  return ((DoubleField*)this->GetField("horizontalFieldOfView"))->GetValue();
}
void Camera::SetHorizontalFieldOfView(double horizontalFieldOfView)
{
	((DoubleField*)this->GetField("horizontalFieldOfView"))->SetValue(horizontalFieldOfView);
}

// Near Clip
double Camera::GetNearClip()
{
  return ((DoubleField*)this->GetField("nearClip"))->GetValue();
}
void   Camera::SetNearClip(double nearClip)
{
	((DoubleField*)this->GetField("nearClip"))->SetValue(nearClip);
}

// Far Clip
double Camera::GetFarClip()
{
  return ((DoubleField*)this->GetField("farClip"))->GetValue();
}
void   Camera::SetFarClip(double farClip)
{
	((DoubleField*)this->GetField("farClip"))->SetValue(farClip);
}

// Render Method
string Camera::GetRenderMethod()
{
  return ((StringField*)this->GetField("renderMethod"))->GetValue();
}
void   Camera::SetRenderMethod(string renderMethod)
{
	((StringField*)this->GetField("renderMethod"))->SetValue(renderMethod);
}

// Z Buffer Depth
int  Camera::GetZBufferDepth()
{
	return ((IntegerField*)this->GetField("zBufferDepth"))->GetValue();
}
void Camera::SetZBufferDepth(int zBufferDepth)
{
	if (( zBufferDepth != 8  )||( zBufferDepth != 16 )||
		( zBufferDepth != 24 )||( zBufferDepth != 32 ))
	{
		((IntegerField*)this->GetField("zBufferDepth"))->SetValue(zBufferDepth);
	}
  else
  {
    throw new SystemException("Camera::SetZBufferDepth","O valor do zBuffer informado e invalido. O valor permitidos: 8,16,24,32 ");
  }
}

// Save Path
string Camera::GetSavePath()
{
  return ((StringField*)this->GetField("savePath"))->GetValue();  
}
void Camera::SetSavePath(string savePath)
{
  ((StringField*)this->GetField("savePath"))->SetValue(savePath);
}

// Save Frames
bool Camera::GetSaveFrames()
{
	return ((BoolField*)this->GetField("saveFrames"))->GetValue();  
}
void Camera::SetSaveFrames(bool saveFrames)
{
  ((BoolField*)this->GetField("saveFrames"))->SetValue(saveFrames);
}

