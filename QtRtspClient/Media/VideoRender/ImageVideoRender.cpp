
#include "ImageVideoRender.h"

ImageVideoRender::ImageVideoRender(QLabel* localVideo, QLabel* remoteVideo)
{
	m_localVideo = localVideo;
	m_remoteVideo = remoteVideo;
	m_bRemoteTranStoped = true;
	connect(this, &ImageVideoRender::localImageReady, this, &ImageVideoRender::OnLocalImageReady);
	connect(this, &ImageVideoRender::remoteImageReady, this, &ImageVideoRender::OnRemoteImageReady);
	//connect(this, SIGNAL(remoteImageReady(const QImage&)), this, SLOT(OnRemoteImageReady(const QImage&)));
	
}
void ImageVideoRender::PlayLocalVideo(unsigned char* data, int videoWidth, int videoHeight)
{
	QImage image(data, videoWidth, videoHeight, QImage::Format_RGB888);
	//emit videoImageReady(image);

	//QImage igScaled = image.scaled(ui.localVideoLabel->width(), ui.localVideoLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	emit localImageReady(image);
	//QPixmap pix = QPixmap::fromImage(image);
	//m_localVideo->setPixmap(pix);
}
void ImageVideoRender::PlayRemoteVideo(unsigned char* data, int videoWidth, int videoHeight)
{
	if (!m_bRemoteTranStoped)
	{
		QImage image(data, videoWidth, videoHeight, QImage::Format_RGB888);

		emit remoteImageReady(image);
		//QPixmap pix = QPixmap::fromImage(image);
		//m_remoteVideo->setPixmap(pix);
	}
}

void ImageVideoRender::SetRemoteTranStoped(bool stoped)
{
	m_bRemoteTranStoped = stoped;
}
void ImageVideoRender::OnLocalImageReady(const QImage& image)
{
	QPixmap pix = QPixmap::fromImage(image);
	pix = pix.scaled(m_localVideo->width(), m_localVideo->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	m_localVideo->setPixmap(pix);
}
void ImageVideoRender::OnRemoteImageReady(const QImage& image)
{
	if (!m_bRemoteTranStoped&& m_remoteVideo)
	{
		QPixmap pix = QPixmap::fromImage(image);

		m_remoteVideo->setPixmap(pix);
	}
}