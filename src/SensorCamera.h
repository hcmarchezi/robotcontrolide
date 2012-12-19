
#ifndef SENSORCAMERA_H
#define SENSORCAMERA_H

#include "Camera.h"

class SensorCamera : public Model
{
public: SensorCamera(void);
public: virtual ~SensorCamera(void);


// update rate (frames per second)
public: double GetUpdateRate();
public: void SetUpdateRate(double updateRate);

// image size
public: IntegerTuple & GetImageSize();
public: void SetImageSize(IntegerTuple imageSize);

// hfov
public: double GetHorizontalFieldOfView();
public: void SetHorizontalFieldOfView(double horizontalFieldOfView);

// near clip
public: double GetNearClip();
public: void SetNearClip(double nearClip);

// far clip
public: double GetFarClip();
public: void SetFarClip(double farClip);

// render method - SGI GLX XLIB AUTO
public: string GetRenderMethod();
public: void SetRenderMethod(string renderMethod);

// valores inteiros - só pode ser 8, 16, 24, 32 bits
public: int GetZBufferDepth();
public: void SetZBufferDepth(int zBufferDepth);

public: string GetSavePath();
public: void SetSavePath(string savePath);

public: bool GetSaveFrames();
public: void SetSaveFrames(bool saveFrames);




// Use Frustum
public: bool GetUseFrustum();
public: void SetUseFrustum(bool useFrustum);

// Min Zoom Limit
public: double GetMinZoom();
public: void SetMinZoom(double minZoom);

// Max Zoom Limit
public: double GetMaxZoom();
public: void SetMaxZoom(double maxZoom);

// Motion Gain
public: double GetMotionGain();
public: void SetMotionGain(double motionGain);

// Zoom Gain
public: double GetZoomGain();
public: void SetZoomGain(double zoomGain);
};

#endif

