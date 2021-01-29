#pragma once

class MediaStatusCallback
{
public:
	virtual void OnCameraOpenFailed() = 0;
	virtual void OnMediaOpened(const int videoWidth, const int videoHeight) = 0;

};
