#pragma once
class MediaDataHandler
{
public:
	virtual ~MediaDataHandler() {}

	//媒体准备好的回调
	virtual  void OnMediaOpenReady(int videoWidth, int videoHeight, int sampleRate, int sampleSize, int channels) = 0;
	//解码后得帧数据准备好的回调
	virtual  void OnMediaFrameDataReady(unsigned char* data, int len, long pts, int type) = 0;
};
