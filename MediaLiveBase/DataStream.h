#pragma once
#define MEDIA_FRAME_TYPE_VIDEO 0
#define MEDIA_FRAME_TYPE_AUDIO 1


class DataStream
{
public:
	virtual ~DataStream() {}

	virtual int Start() = 0;
	virtual int Stop() = 0;
};
