#pragma once
#include "MediaDataHandler.h"

class MediaStreamDataHandler:public MediaDataHandler
{
public:
	virtual ~MediaStreamDataHandler() {}

	//����ͷ��ʧ��
	virtual  void OnCameraOpenFailed() = 0;
	//֡����׼���õĻص�;
	virtual  void OnMediaPacketDataReady(unsigned char* data, int len, long pts, int type) = 0;
};
