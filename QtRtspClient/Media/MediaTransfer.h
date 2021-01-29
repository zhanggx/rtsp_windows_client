#pragma once

#define FnMediaReceiveCallBack

class MediaTransfer
{
public:
	virtual void Start(void* userData);// , FnMediaReceiveCallBack videoCallBack, FnMediaReceiveCallBack audioCallBack) = 0;
	virtual void Stop() = 0;
	virtual int SendVideoPacket(unsigned char* data, int len, long pts) = 0;
	virtual int SendAudioPacket(unsigned char* data, int len, long pt) = 0;
};
