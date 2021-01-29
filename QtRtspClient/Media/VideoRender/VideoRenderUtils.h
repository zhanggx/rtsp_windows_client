#pragma once
#define VIDEO_RENDER_IMAGE 0
#define VIDEO_RENDER_GDI 1

#include <QLabel>

#include "VideoRender.h"

VideoRender* createVideoRender(int type,QLabel* localVideo, QLabel* remoteVideo);
