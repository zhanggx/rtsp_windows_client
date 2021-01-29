#include "RtspDataSource.h"
#include <windows.h>

#include "DataStreamExport.h"


int maxCount=0;
RtspDataSource::RtspDataSource(MediaController* pController, const char* url, bool tcpTransport):m_pController(pController),mUrl(url){

	m_Running=false;
	m_pMediaTransfer=NULL;
	m_pAudioPlayer = NULL;
	m_pVideoRender = NULL;
	m_nSrcVideoWidth = 0; 
	m_nSrcVideoHeight = 0;
	m_pDataStream = CreateDataStream(url, tcpTransport,this);
	connect(this, &QThread::finished, this, &QObject::deleteLater);
}
RtspDataSource::~RtspDataSource() {
	delete m_pDataStream;
	m_pDataStream = NULL;
}

int AVInterruptCallBackFun(void* param)
{
	RtspDataSource* pRtspDataSource = (RtspDataSource*)param;
	if (NULL == pRtspDataSource) return 0;

	if (pRtspDataSource->IsRunning())
	{
		//通知FFMpeg继续阻塞工作
		return 0;
	}
	else
	{
		//通知FFMpeg可以从阻塞工作线程中释放操作
		return 1;
	}
}

void mySleep(int sleepMS) {
	if (sleepMS > 0) {
		Sleep(sleepMS * 1000);
	}
}
//7表示SPS,8表示PPS 1表示P帧
int GetH264NalType(uint8_t* data)
{
	if (data[0]==0&& data[1] == 0 && data[2] == 0)
	{
		if(data[3] == 0&& data[4] == 1)
		{
			return data[5]&0X1F;
		}
		else if (data[3] == 1)
		{
			return data[4] & 0X1F;
		}		
	}
	return -1;
}
//摄像头打开失败
void RtspDataSource::OnCameraOpenFailed()
{
	emit cameraOpenFailed();
}
//媒体准备好的回调
void RtspDataSource::OnMediaOpenReady(int videoWidth, int videoHeight, int sampleRate, int sampleSize, int channels)
{
	m_nSrcVideoWidth = videoWidth;
	m_nSrcVideoHeight = videoHeight;
	emit mediaOpened(videoWidth, videoHeight);
}
//帧数据准备好的回调;
void RtspDataSource::OnMediaPacketDataReady(unsigned char* data, int len, long pts, int type)
{
	if (m_pMediaTransfer) {
		if (type== MEDIA_FRAME_TYPE_VIDEO)
		{
			/*int naltype = GetH264NalType(data);
			if (naltype == 5)//没有带上ｓｐｓ的I帧
			{
				memcpy(tempData, pVideoCodecCtx->extradata, pVideoCodecCtx->extradata_size);
				memcpy(tempData + pVideoCodecCtx->extradata_size, pkt.data, pkt.size);
				m_pMediaTransfer->SendVideoPacket(tempData, pkt.size + pVideoCodecCtx->extradata_size, pkt.pts);
			}
			else
			{
				m_pMediaTransfer->SendVideoPacket(pkt);
			}*/
			m_pMediaTransfer->SendVideoPacket(data, len, pts);
		}
		else if (type == MEDIA_FRAME_TYPE_AUDIO)
		{
				m_pMediaTransfer->SendAudioPacket(data,len,pts);
		}
	}
}
//解码后得帧数据准备好的回调
void RtspDataSource::OnMediaFrameDataReady(unsigned char* data, int len, long pts, int type)
{
	if (type == MEDIA_FRAME_TYPE_VIDEO)
	{						//发送获取一帧图像信号
		if (m_pVideoRender)
		{
			m_pVideoRender->PlayLocalVideo(data, m_nSrcVideoWidth, m_nSrcVideoHeight);
		}
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