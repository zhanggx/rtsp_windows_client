#pragma once
#include <string>

#include "DataStream.h"
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
#define LITTLE_ARRAY_LEN                 128                //小型数组的长度

class MediaDataStream :public DataStream
{
public:
	MediaDataStream(const char* url, bool tcpTransport, MediaStreamDataHandler* pHandler);
	~MediaDataStream();
	int Start() override;
	int Stop() override;
public:
	bool IsRunning()
	{
		return m_bRunning;
	}
private:
	void* OpenRtsp();
private:
	bool m_bRunning;
	std::string mUrl;
	bool m_bTcpTransport;
	MediaStreamDataHandler* m_pHandler;
};

