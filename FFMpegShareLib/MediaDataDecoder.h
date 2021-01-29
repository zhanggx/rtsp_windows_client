#pragma once
#include "MediaDecoder.h"
#include "MediaStreamDataHandler.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/time.h"
}

class MediaDataDecoder :public MediaDecoder
{
public:
	MediaDataDecoder(MediaDataHandler* pHandler);
	~MediaDataDecoder();
public:
	int Init(int nDstVideoWidth, int nDstVideoHeight) override;
	int Decode(MediaData* pMediaData) override;
	int Destory() override;
	void Stop() override
	{
		m_bDestroyed = true;
	}
	int GetFrameCount() override
	{
		return m_nFrameCount;
	}
private:
	AVPacket pkt;
	AVPicture  pAVPicture;
	SwsContext* pSwsContext;
	SwsContext* pAudioSwsContext;
	SwrContext* pAudioSwrContext;
	AVCodecContext* pVideoCodecCtx;
	AVCodecContext* pAudioCodecCtx;
	AVFrame* pAVFrame;
	uint8_t* out_buffer;
	int out_linesize;
	int out_buffer_size;
	int m_nFrameCount;
	bool m_bDestroyed;
	int m_nDstVideoWidth;
	int m_nDstVideoHeight;
	int videoWidth;
	int videoHeight;
	MediaDataHandler* m_pHandler;
};

