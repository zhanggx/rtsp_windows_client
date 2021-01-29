#pragma once
#include "MediaDataHandler.h"

class MediaStreamDataHandler:public MediaDataHandler
{
public:
	virtual ~MediaStreamDataHandler() {}

	//摄像头打开失败
	virtual  void OnCameraOpenFailed() = 0;
	//帧数据准备好的回调;
	virtual  void OnMediaPacketDataReady(unsigned char* data, int len, long pts, int type) = 0;
};
