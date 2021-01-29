#include "QtRtspClient.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include "QtSettingDialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load("language_zh");
    a.installTranslator(&translator);
    QtSettingDialog settingDialog;
    int res = settingDialog.exec();
    int result = 0;
    if (res == QDialog::Accepted) {
        QString rtspUrl = settingDialog.GetRtspUrl();
        QString videoRender = settingDialog.GetVideoRender();
        QtRtspClient w(Q_NULLPTR, rtspUrl, videoRender,settingDialog.IsTcpTransport());
        w.show();
        result = a.exec();
    }
    return result;
}
