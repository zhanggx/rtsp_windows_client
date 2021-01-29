#pragma once

#pragma once
#include <iostream>
#include <qobject.h>
#include <QThread.h>
#include <string.h>

#include "MediaTransfer.h"
#include <QImage>
#include <QQueue>
#include <QMutex>

#include "AudioPlayer.h"
#include "VideoRender.h"
#include "MediaDecoder.h"
#include "MediaDataHandler.h"

#define LITTLE_ARRAY_LEN                 128                //小型数组的长度
class MediaController;



class RtspDataDecoder : public QThread, public MediaDataHandler
{
	Q_OBJECT
		void run() override {
		m_Running = true;
		int result = ReadData();
		emit resultReady(result);
	}
public:
	RtspDataDecoder(MediaController* pController);
	~RtspDataDecoder();

	//媒体准备好的回调
	void OnMediaOpenReady(int videoWidth, int videoHeight, int sampleRate, int sampleSize, int channels) override;
	//解码后得帧数据准备好的回调
	void OnMediaFrameDataReady(unsigned char* data, int len, long pts, int type) override;
public:
	void Start(int nDstVideoWidth, int nDstVideoHeight)
	{
		this->m_nDstVideoWidth= nDstVideoWidth;
		this->m_nDstVideoHeight = nDstVideoHeight;
		this->start();
	}
	void Stop()
	{
		m_pMediaDecoder->Stop();
		this->quit();
		m_Running = false;
		this->wait();
	}
	void SetAudioPlayer(AudioPlayer* pAudioPlayer)
	{
		m_pAudioPlayer = pAudioPlayer;
	}
	void SetVideoRender(VideoRender* pVideoRender)
	{
		m_pVideoRender = pVideoRender;
	}
	/*
	void AddMediaData(ESH_BitStream* stream,int type)
	{
		MediaData* pMediaData=new MediaData;
		pMediaData->len=stream->len;
		pMediaData->type = type;
		pMediaData->dts = stream->dts;
		pMediaData->pts = stream->pts;
		pMediaData->buf = (unsigned char*)malloc(stream->len);
		memcpy(pMediaData->buf, stream->buf,stream->len);
		m_mutex.lock();
		m_mediaQueue.enqueue(pMediaData);
		m_mutex.unlock();
	}*/
private:
	int ReadData();
	int m_nDstVideoWidth;
	int m_nDstVideoHeight;
	bool m_Running;
	MediaController* m_pController;
	AudioPlayer* m_pAudioPlayer;
	QQueue<MediaData*> m_mediaQueue;
	QMutex m_mutex;
	MediaDecoder* m_pMediaDecoder;
	VideoRender* m_pVideoRender;
signals:
	void resultReady(const int result);
};

