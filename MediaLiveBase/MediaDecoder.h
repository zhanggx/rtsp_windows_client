#pragma once

typedef struct
{
	unsigned char* buf;
	int len;
	int type;  //0 ”∆µ 1“Ù∆µ
	long long		pts;
	long long		dts;
} MediaData;

class MediaDecoder
{
public:
	virtual ~MediaDecoder() {}

	virtual int Init(int nDstVideoWidth, int nDstVideoHeight) = 0;
	virtual int Decode(MediaData* pMediaData) = 0;
	virtual int Destory() = 0;
	virtual void Stop() = 0;
	virtual int GetFrameCount() = 0;
};
