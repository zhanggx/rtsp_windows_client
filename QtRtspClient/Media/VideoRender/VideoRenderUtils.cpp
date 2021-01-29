
#include "VideoRender.h"

#include "ImageVideoRender.h"
#include "GdiVideoRender.h"
#include "VideoRenderUtils.h"

VideoRender* createVideoRender(int type, QLabel* localVideo, QLabel* remoteVideo)
{
	if (type == VIDEO_RENDER_GDI)
	{
		return new GdiVideoRender(localVideo, remoteVideo);
	}
	return new ImageVideoRender(localVideo, remoteVideo);
	//
}
