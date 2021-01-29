#include "pch.h"

#include "DataStreamExport.h"

#include "MediaDataDecoder.h"
#include "MediaDataStream.h"

DataStream* CreateDataStream(const char* rtsp, bool tcpTransport, MediaStreamDataHandler* pHandler)
{
	return new MediaDataStream(rtsp, tcpTransport, pHandler);
}


MediaDecoder* CreateDataDecoder(MediaDataHandler* pHandler)
{
	return new MediaDataDecoder(pHandler);
}