#pragma once

#include <QLabel>
#include "VideoRender.h"
#include <windows.h>

class GdiVideoRender : public VideoRender
{
public:
	GdiVideoRender(QLabel* localVideo, QLabel* remoteVideo);
	void PlayLocalVideo(unsigned char* data, int videoWidth, int videoHeight);
	void PlayRemoteVideo(unsigned char* data, int videoWidth, int videoHeight);	
	void SetRemoteTranStoped(bool stoped);
private:
	void ReaderVideoData(HWND hwnd, unsigned char* data, int videoWidth, int videoHeight);
private:
	QLabel* m_remoteVideo;
	QLabel* m_localVideo;
	HWND m_localHwnd;
	HWND m_remoteHwnd;
	bool m_bRemoteTranStoped;
};
