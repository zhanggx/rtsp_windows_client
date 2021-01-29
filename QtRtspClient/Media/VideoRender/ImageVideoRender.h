#pragma once

#include <QLabel>
#include "VideoRender.h"

class ImageVideoRender : public QObject, public VideoRender
{
	Q_OBJECT
public:
	ImageVideoRender(QLabel* localVideo, QLabel* remoteVideo);
	void PlayLocalVideo(unsigned char* data, int videoWidth, int videoHeight);
	void PlayRemoteVideo(unsigned char* data, int videoWidth, int videoHeight);	
	void SetRemoteTranStoped(bool stoped);
private:
	QLabel* m_remoteVideo;
	QLabel* m_localVideo;
	bool m_bRemoteTranStoped;
public slots:
	void OnLocalImageReady(const QImage& image);
    void OnRemoteImageReady(const QImage& image);
signals:
	void localImageReady(const QImage& image);
    void remoteImageReady(const QImage& image);
};
