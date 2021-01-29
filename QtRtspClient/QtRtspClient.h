#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtRtspClient.h"
#include "Media/MediaController.h"
#include "Media/MediaStatusCallback.h"

class QtRtspClient : public QMainWindow, public MediaStatusCallback
{
    Q_OBJECT

public:
    QtRtspClient(QWidget *parent,QString& rtspUrl, QString& videoRender,bool tcpTransport);

public:
    void OnCameraOpenFailed() override;
    void OnMediaOpened(const int videoWidth, const int videoHeight) override;
protected:
    virtual void showEvent(QShowEvent*);
    virtual void closeEvent(QCloseEvent* event);
private:
    void InitStatusBar();

private:
    Ui::QtRtspClientClass ui;
    int m_nRtspMediaStatus;
    std::string m_rtspUrl;
    VideoRender* m_pVideoRender;
    MediaController m_mediaController;
    QLabel *m_pRtspStatusLabel,*m_pRtspUrlLabel;
public slots:
    void OnSetupMenuClicked();
    void OnTestButtonClicked();
    void handleMediaResults(const int result);
};
