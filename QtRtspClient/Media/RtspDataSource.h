#pragma once
#include <iostream>
#include <qobject.h>
#include <QThread.h>
#include <string.h>

#include "MediaTransfer.h"
#include <QImage.h>

#include "AudioPlayer.h"
#include "DataStream.h"
#include "VideoRender.h"
#include "MediaStreamDataHandler.h"

#define LITTLE_ARRAY_LEN                 128                //С������ĳ���
class MediaController;

class RtspDataSource : public QThread, public MediaStreamDataHandler
{
	Q_OBJECT
		void run() override {
		m_Running=true;
		int result= m_pDataStream->Start();
		emit resultReady(result);
	}
public:
	RtspDataSource(MediaController* pController,const char* url, bool tcpTransport);
	virtual ~RtspDataSource();

	//����ͷ��ʧ��
	void OnCameraOpenFailed() override;
	//ý��׼���õĻص�
	void OnMediaOpenReady(int videoWidth, int videoHeight, int sampleRate, int sampleSize, int channels) override;
	//֡����׼���õĻص�;
	void OnMediaPacketDataReady(unsigned char* data, int len, long pts, int type) override;
	//������֡����׼���õĻص�
	void OnMediaFrameDataReady(unsigned char* data, int len, long pts, int type) override;
public:
	void Start()
	{
		this->start();
	}
	void Stop()
	{
		m_pDataStream->Stop();
		this->quit();
		m_Running = false;
		this->wait();
	}
	std::string& GetUrl()
	{
		return mUrl;
	}
	void SetMediaTransfer(MediaTransfer* pMediaTransfer)
	{
		m_pMediaTransfer= pMediaTransfer;
	}
	void SetAudioPlayer(AudioPlayer* pAudioPlayer)
	{
		m_pAudioPlayer = pAudioPlayer;
	}
	void SetVideoRender(VideoRender* pVideoRender)
	{
		m_pVideoRender = pVideoRender;
	}
	bool IsRunning() {
		if (!m_Running) {
			return false;
		}
		
		return m_Running;
	}
private:

	std::string mUrl;
	bool m_Running;
	int m_nSrcVideoWidth, m_nSrcVideoHeight;
	MediaTransfer* m_pMediaTransfer;
	MediaController* m_pController;
	AudioPlayer* m_pAudioPlayer;
	VideoRender* m_pVideoRender;
	DataStream* m_pDataStream;
signals:
	void resultReady(const int result);
	void cameraOpenFailed();
	void mediaOpened(const int videoWidth,const int videoHeight);
};

