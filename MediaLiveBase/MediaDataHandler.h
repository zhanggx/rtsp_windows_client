#pragma once
class MediaDataHandler
{
public:
	virtual ~MediaDataHandler() {}

	//ý��׼���õĻص�
	virtual  void OnMediaOpenReady(int videoWidth, int videoHeight, int sampleRate, int sampleSize, int channels) = 0;
	//������֡����׼���õĻص�
	virtual  void OnMediaFrameDataReady(unsigned char* data, int len, long pts, int type) = 0;
};
