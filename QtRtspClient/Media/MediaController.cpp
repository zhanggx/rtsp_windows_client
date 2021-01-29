#include "MediaController.h"
#include <QPixmap.h>

#include "../QtRtspClient.h"
#include "MediaControllerThread.h"

#define THREAD_FUN_TYPE_LOGIN 1
#define THREAD_FUN_TYPE_HEARTBEAT 2

#define LOCAL_VIDEO_PORT 11000
#define LOCAL_AUDIO_PORT 11002
#pragma region 异步处理方法的声明
int ConnectLogin(MediaController* pController, int type, void* pParam);
int SendHeartBeat(MediaController* pController, int type, void* pParam);
#pragma endregion

MediaController::MediaController():m_pVideoRender(NULL), m_bLogined(false), m_pMediaTransfer(NULL), m_pMediaStatusCallback(NULL)
{
	m_nRemoteVideoWidth=1280;
	m_nRemoteVideoHeight=720;
	m_bRemoteTranStoped = true;
	m_bTcpTransport = false;
	m_pRtspSource = NULL;
	//PreferenceUtils::GetString(KEY_ACCOUNT, m_account);
	m_pTimer=new QTimer(this);
	//m_localIp= GetIpAddressQ().toLocal8Bit();
	m_pMediaDecoder=new RtspDataDecoder(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	connect(m_pMediaDecoder, &RtspDataDecoder::resultReady, this, &MediaController::handleDecoderResults);
}
MediaController::~MediaController()
{
	//delete m_pMediaDecoder;
	//m_pMediaDecoder=NULL;
}
void MediaController::Init(VideoRender* pVideoRender, bool tcpTransport, MediaStatusCallback* pMediaStatusCallback)
{
	m_pVideoRender = pVideoRender;
	m_bTcpTransport = tcpTransport;
	m_pMediaStatusCallback = pMediaStatusCallback;
	if (m_pMediaDecoder)
	{
		m_pMediaDecoder->SetVideoRender(pVideoRender);
	}
}
void MediaController::Start(const char* rtspUrl, int remoteVideoWidth, int remoteVideoHeight)
{
	qDebug("MediaController::StartCCSServer");
	m_nRemoteVideoWidth = remoteVideoWidth;
	m_nRemoteVideoHeight = remoteVideoHeight;
	m_rtspUrl=rtspUrl;
	assert(!m_rtspUrl.empty());
	//MediaControllerThread* pThread=new MediaControllerThread(this, ConnectLogin, THREAD_FUN_TYPE_LOGIN,&m_ccsManager);

	//connect(pThread, &MediaControllerThread::OnThreadEnd, this, &MediaController::asyncThreadEnded);
	//pThread->start();
	if (m_pTimer)
	{
		m_pTimer->start(8000);
	}
	StartRtsp();
}
void MediaController::Stop()
{
	if (m_pTimer)
	{
		m_pTimer->stop();
	}
	m_bRemoteTranStoped = true;
	m_pVideoRender->SetRemoteTranStoped(true);
	StopRtsp();
	CloseMediaTransfer();
	m_pMediaDecoder->Stop();
}
void MediaController::StartRtsp()
{
	if(m_pRtspSource)
	{
		StopRtsp();
	}
	m_pRtspSource=new RtspDataSource(this, m_rtspUrl.c_str(), m_bTcpTransport);
	//m_pRtspSource->SetAudioPlayer(&m_audioPlayer);
	connect(m_pRtspSource, &RtspDataSource::resultReady, this, &MediaController::handleResults);
	connect(m_pRtspSource, &RtspDataSource::cameraOpenFailed, this, &MediaController::OnCameraOpenFailed);
	connect(m_pRtspSource, &RtspDataSource::mediaOpened, this, &MediaController::OnMediaOpened);
	m_pRtspSource->SetVideoRender(m_pVideoRender);
	m_pRtspSource->Start();

}
void MediaController::StartTest()
{
	StartMediaTransfer("127.0.0.1", LOCAL_VIDEO_PORT, LOCAL_AUDIO_PORT);
}
void MediaController::StopRtsp()
{
	if (m_pRtspSource)
	{
		m_pRtspSource->SetMediaTransfer(NULL);
		m_pRtspSource->SetVideoRender(NULL);
		m_pRtspSource->Stop();
		delete m_pRtspSource;
		m_pRtspSource = NULL;
	}
}
void MediaController::handleResults(const int result)
{
	qDebug("MediaController::handleResults#result=%d",result);
	StopRtsp();
	emit OnMediaStoped(result);
}
void MediaController::handleDecoderResults(const int result)
{
	qDebug("MediaController::handleDecoderResults#result=%d", result);
}

void MediaController::OnCameraOpenFailed()
{
	qDebug("MediaController::OnCameraOpenFailed");
	if (m_pMediaStatusCallback)
	{
		m_pMediaStatusCallback->OnCameraOpenFailed();
	}
}
void MediaController::OnMediaOpened(const int videoWidth, const int videoHeight)
{
	qDebug("MediaController::OnMediaOpened#videoWidth=%d,videoHeight=%d", videoWidth, videoHeight);
	if (m_pMediaStatusCallback)
	{
		m_pMediaStatusCallback->OnMediaOpened(videoWidth, videoHeight);
	}
}
void MediaController::asyncThreadEnded(const void* thread, const int type, const int result)
{
	qDebug("MediaController::asyncThreadEnded#type=%d,result=%d",type,result);
	if (THREAD_FUN_TYPE_LOGIN==type)
	{
		//是否登录成功
		
	}
	
}
void MediaController::timerUpdate()
{
	if (m_bLogined)
	{
		//MediaControllerThread* pThread = new MediaControllerThread(this, SendHeartBeat, THREAD_FUN_TYPE_HEARTBEAT, &m_ccsManager);
		//pThread->start();
	}
	else
	{
		//MediaControllerThread* pThread = new MediaControllerThread(this, ConnectLogin, THREAD_FUN_TYPE_LOGIN, &m_ccsManager);

		//connect(pThread, &MediaControllerThread::OnThreadEnd, this, &MediaController::asyncThreadEnded);
		//pThread->start();
	}
}
void MediaController::CloseMediaTransfer()
{
	if (m_pRtspSource)
	{
		m_pRtspSource->SetMediaTransfer(NULL);
	}
	if (m_pMediaTransfer)
	{
		m_pMediaTransfer->Stop();
		delete m_pMediaTransfer;
		m_pMediaTransfer=NULL;
	}
}
void MediaController::StartMediaTransfer(const char* servip, int videoPort, int audioPort)
{
	if (!m_bLogined)
	{
		return;
	}
	CloseMediaTransfer();
	//m_pMediaTransfer=new EshMediaTransfer(servip,videoPort,audioPort, LOCAL_VIDEO_PORT,LOCAL_AUDIO_PORT);
	//m_pMediaTransfer->Start(this, videoFuncdata,audioFuncdata);
	if (m_pRtspSource)
	{
		m_pRtspSource->SetMediaTransfer(m_pMediaTransfer);
	}
	m_pMediaDecoder->SetAudioPlayer(&m_audioPlayer);
	m_pMediaDecoder->Start(m_nRemoteVideoWidth, m_nRemoteVideoHeight);
	m_bRemoteTranStoped = false;
	m_pVideoRender->SetRemoteTranStoped(false);
}