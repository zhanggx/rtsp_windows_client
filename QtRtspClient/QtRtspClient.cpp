#include "QtRtspClient.h"
#include "Media/VideoRender/VideoRenderUtils.h"
#include <QCloseEvent>
#include <QMessageBox>

#define MEDIA_STATUS_NONE 0
#define MEDIA_STATUS_CONNECTING 1
#define MEDIA_STATUS_CONNECTED 2
#define MEDIA_STATUS_CONNECT_FAILED 3

QtRtspClient::QtRtspClient(QWidget* parent, QString& rtspUrl, QString& videoRender, bool tcpTransport)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_rtspUrl = rtspUrl.toStdString(); //; "rtsp://192.168.1.141:554/live/av0";
	m_pRtspStatusLabel = NULL;
	m_pRtspUrlLabel = NULL;
	InitStatusBar();
	connect(&m_mediaController, &MediaController::OnMediaStoped, this, &QtRtspClient::handleMediaResults);
	connect(ui.actionSetup, SIGNAL(triggered()), this, SLOT(OnSetupMenuClicked()));
	int videoReaderType = videoRender.compare("gdi", Qt::CaseInsensitive) ? VIDEO_RENDER_IMAGE : VIDEO_RENDER_GDI;
	m_pVideoRender = createVideoRender(videoReaderType, ui.localVideoLabel, NULL);
	m_mediaController.Init(m_pVideoRender, tcpTransport,this);
}

void QtRtspClient::InitStatusBar()
{
	//ui.statusBar->setSizeGripEnabled(false);//去掉状态栏右下角的三角
	//ui.statusBar->showMessage(QString::fromStdString(m_rtspUrl), 2000);//显示临时信息2000ms 前面的正常信息被覆盖 当去掉后一项时，会一直显示

	m_pRtspUrlLabel = new QLabel(QString::fromStdString(m_rtspUrl), this);
	QSize size1(m_pRtspUrlLabel->sizeHint().width()+20, m_pRtspUrlLabel->sizeHint().height());
	m_pRtspUrlLabel->setMinimumSize(size1);
	ui.statusBar->addWidget(m_pRtspUrlLabel);

	m_pRtspStatusLabel = new QLabel(tr("rtsp_connecting"), this);
	QSize size2(m_pRtspStatusLabel->sizeHint().width() + 20, m_pRtspStatusLabel->sizeHint().height());
	m_pRtspStatusLabel->setMinimumSize(size2);
	ui.statusBar->addWidget(m_pRtspStatusLabel);
	/*
	QLabel* permanent = new QLabel(this);
	permanent->setAlignment(Qt::AlignLeft);
	permanent->setMinimumSize(permanent->sizeHint());
	//permanent->setOpenExternalLinks(true);//设置可以打开网站链接
	ui.statusBar->addPermanentWidget(permanent);//显示永久信息*/

	//ui.statusBar->showMessage(QString::fromStdString(m_rtspUrl));
}
void QtRtspClient::showEvent(QShowEvent* _event)
{
	static bool shown = false;
	if (!shown) {//只执行一次
		//PreferenceUtils::GetString(KEY_RTSP_URL, m_rtspUrl);
		//PreferenceUtils::GetString(KEY_ACCOUNT, m_account);
		if (!m_rtspUrl.empty())
		{
			m_mediaController.Start(m_rtspUrl.c_str(),  0, 0);
				//ui.localVideoLabel->width(), ui.localVideoLabel->height(), 0, 0);
			//ui.remoteVideoLabel->width(), ui.remoteVideoLabel->height());
		}
		shown = true;
	}
}
void QtRtspClient::closeEvent(QCloseEvent* event)
{
	int result = QMessageBox::question(this, tr("window_title"),
		tr("confirm_exit"),
		tr("yes"), tr("no"),
		0, 1);
	if (result == 0)
	{
		m_mediaController.Stop();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}
void QtRtspClient::OnSetupMenuClicked()
{

	/*if (m_nRtspMediaStatus== MEDIA_STATUS_CONNECTED)
	{
		m_mediaController.StopRtsp();
		m_nRtspMediaStatus = MEDIA_STATUS_NONE;
		ui.startButton->setText("Start");
		return;
	}
	if (m_nRtspMediaStatus == MEDIA_STATUS_NONE)
	{
		m_nRtspMediaStatus= MEDIA_STATUS_CONNECTING;
		m_mediaController.StartRtsp();
		ui.startButton->setText("Connecting");
	}*/
}
void QtRtspClient::OnTestButtonClicked()
{

	m_mediaController.StartTest();
	//ui.testlabel->setText(QAudioDeviceInfo::defaultOutputDevice().deviceName());

	/*QList<QAudioDeviceInfo> audioDeviceListO = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	//QAudioDeviceInfo devInfo= audioDeviceListO[0];
	//ui.testlabel->setText(devInfo.deviceName());
	foreach(QAudioDeviceInfo devInfo, audioDeviceListO)
	{
		QString strName = devInfo.deviceName();
		QStringList list = devInfo.supportedCodecs();
		QString str = list.join(",");
		qDebug("supportedCodecs#name=%s,codec=%s,realm=%s", qPrintable(strName), qPrintable(str), qPrintable(devInfo.realm()));

	}*/
}
void QtRtspClient::handleMediaResults(const int result)
{
	qDebug("QtRtspClient::handleResults#result=%d", result);
	m_nRtspMediaStatus = MEDIA_STATUS_NONE;
	//ui.startButton->setText(tr("start"));
}

void QtRtspClient::OnCameraOpenFailed()
{
	qDebug("QtRtspClient::OnCameraOpenFailed");

	m_nRtspMediaStatus = MEDIA_STATUS_CONNECT_FAILED;
	if (m_pRtspStatusLabel) {
		m_pRtspStatusLabel->setText(tr("rtsp_connect_failed"));
	}
}
void QtRtspClient::OnMediaOpened(const int videoWidth, const int videoHeight)
{
	qDebug("QtRtspClient::OnMediaOpened#videoWidth=%d,videoHeight=%d", videoWidth, videoHeight);

	m_nRtspMediaStatus = MEDIA_STATUS_CONNECTED;
	//ui.startButton->setText(tr("stop"));
	if (m_pRtspStatusLabel) {
		m_pRtspStatusLabel->setText(tr("rtsp_connected"));
	}
}