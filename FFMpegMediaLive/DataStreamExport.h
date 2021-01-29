#pragma once
#include <DataStream.h>
#include "MediaDecoder.h"
#include "MediaStreamDataHandler.h"

#ifdef FFMPEGMEDIALIVELIB_EXPORTS
#define MEDIALIB_API __declspec(dllexport)
#else
#define MEDIALIB_API __declspec(dllimport)
#endif

extern "C"  MEDIALIB_API DataStream * CreateDataStream(const char* rtsp, bool tcpTransport, MediaStreamDataHandler * pHandler);

extern "C"  MEDIALIB_API MediaDecoder * CreateDataDecoder(MediaDataHandler * pHandler);
