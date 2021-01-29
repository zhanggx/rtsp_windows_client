#pragma once

class VideoRender
{
public:
	virtual void PlayLocalVideo(unsigned char* data, int videoWidth, int videoHeight) = 0;
	virtual void PlayRemoteVideo(unsigned char* data, int videoWidth, int videoHeight) = 0;
	virtual void SetRemoteTranStoped(bool stoped) = 0;
};
