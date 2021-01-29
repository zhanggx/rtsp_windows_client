#include "RtspDataDecoder.h"

#include "DataStreamExport.h"


RtspDataDecoder::RtspDataDecoder(MediaController* pController) :m_pController(pController), m_pVideoRender(NULL)
{

	m_Running = false;
	m_pAudioPlayer=NULL;
	m_pMediaDecoder= CreateDataDecoder(this);
	connect(this, &QThread::finished, this, &QObject::deleteLater);
}
RtspDataDecoder::~RtspDataDecoder() {
	delete m_pMediaDecoder;
	m_pMediaDecoder = NULL;
}

//媒体准备好的回调
void RtspDataDecoder::OnMediaOpenReady(int videoWidth, int videoHeight, int sampleRate, int sampleSize, int channels)
{
	if (m_pAudioPlayer)
	{
		m_pAudioPlayer->Start(sampleRate, sampleSize, channels);
	}
}

//解码后得帧数据准备好的回调
void RtspDataDecoder::OnMediaFrameDataReady(unsigned char* data, int len, long pts, int type)
{
	if (type == MEDIA_FRAME_TYPE_VIDEO)
	{
		if (m_pVideoRender)
		{
			m_pVideoRender->PlayRemoteVideo(data, m_nDstVideoWidth, m_nDstVideoHeight);
		}
		//QImage image(data, m_nDstVideoWidth, m_nDstVideoHeight, QImage::Format_RGB888);
		//emit videoImageReady(image);
	}
	else if (type == MEDIA_FRAME_TYPE_AUDIO)
	{
		if (m_pAudioPlayer)
		{
			m_pAudioPlayer->Play((const char*)data, len);
			//m_pAudioPlayer->Play((const char*)pAVFrame->data[0], size);
		}
	}
}

int RtspDataDecoder::ReadData() {
	
	m_pMediaDecoder->Init(m_nDstVideoWidth, m_nDstVideoHeight);
	//av_init_packet(&videoPkt)
	while (!isFinished() && isRunning() && m_Running) {
		//int64_t mReadFrameTime = av_gettime();
		MediaData* pMediaData =NULL;
		m_mutex.lock();
		if (!m_mediaQueue.isEmpty())
		{
			pMediaData=m_mediaQueue.dequeue();
		}
		m_mutex.unlock();
		if (!pMediaData)
		{
			continue;
		}
		m_pMediaDecoder->Decode(pMediaData);
		free(pMediaData->buf);
		pMediaData->buf = NULL;
		delete pMediaData;
	}
	m_pMediaDecoder->Destory();

	return 0;
}