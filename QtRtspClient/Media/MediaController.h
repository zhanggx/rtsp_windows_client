#pragma once
#include <qobject.h>
#include <QThread.h>
#include <QTimer>

#include "VideoRender.h"
#include "RtspDataSource.h"
#include "AudioPlayer.h"
#include "MediaStatusCallback.h"
#include "RtspDataDecoder.h"


class MediaController : public QObject
{
    Q_OBJECT
public:
    MediaController();
    ~MediaController();

public:
	void Init(VideoRender* pVideoRender, bool tcpTransport, MediaStatusCallback* pMediaStatusCallback);
    void Start(const char* rtspUrl, int remoteVideoWidth, int remoteVideoHeight);
    void Stop();
	void StartTest();
	bool IsLogined()
	{
		return m_bLogined;
	}
    RtspDataDecoder* GetMediaDecoder()
	{
		return m_pMediaDecoder;
	}
private:
    RtspDataSource* m_pRtspSource;
    VideoRender* m_pVideoRender;
    MediaStatusCallback* m_pMediaStatusCallback;
    AudioPlayer m_audioPlayer;
    RtspDataDecoder* m_pMediaDecoder;
    MediaTransfer* m_pMediaTransfer;
    std::string m_account;
    std::string m_localIp;
    std::string m_rtspUrl;
	std::string m_mainSpeakerId;
	bool m_bLogined;
    bool m_bRemoteTranStoped;
    bool m_bTcpTransport;
    int m_nRemoteVideoWidth;
    int m_nRemoteVideoHeight;
	QTimer* m_pTimer;
private:
    void StartRtsp();
    void StopRtsp();
	void CloseMediaTransfer();
    void StartMediaTransfer(const char* servip, int videoPort, int audioPort);
public slots:
    void handleResults(const int result);
    void handleDecoderResults(const int result);
    void OnCameraOpenFailed();
    void OnMediaOpened(const int videoWidth, const int videoHeight);
    void asyncThreadEnded(const void* thread, const int type, const int result);
    void timerUpdate();
signals:
    void OnMediaStoped(const int result);
};
