#include "QtSettingDialog.h"
#include <QMessageBox>
#include <QSettings>
#include "Utils.h"

QtSettingDialog::QtSettingDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	connect(ui.okButton, &QPushButton::clicked, this, &QtSettingDialog::OnConfirmButtonClicked);
	std::string strInitPath = Utils::getAppDataRoamingPath();
	strInitPath += "config.ini";
	m_strInitPath= QString::fromStdString(strInitPath);
	QSettings configIni(m_strInitPath, QSettings::IniFormat);

	QString strUrl = configIni.value("rtsp/url").toString();
	ui.rtspUrlEdit->setText(strUrl);
	QString strTransport =configIni.value("rtsp/transport").toString();
	if (!strTransport.compare("tcp", Qt::CaseInsensitive))
	{
		ui.rbTcpTransport->setChecked(true);
	}
	else {
		ui.rbUdpTransport->setChecked(true);
	}
	QString strRender = configIni.value("video/render").toString();
	if (!strRender.compare("gdi", Qt::CaseInsensitive))
	{
		ui.rbGdiRender->setChecked(true);
	}
	else
	{
		ui.rbImageReader->setChecked(true);
	}
}

QtSettingDialog::~QtSettingDialog()
{
}

void QtSettingDialog::OnConfirmButtonClicked()
{
	//QMessageBox::about(this,"tsststhi","gsdgsdhsdhd");
	QString ccs = ui.rtspUrlEdit->text().trimmed();
	if (ccs.isEmpty())
	{
		QMessageBox::about(this, tr("setting_dialog_title"), tr("rtsp_url_request"));
		return;
	}
	/*
	QString ccsPort = ui.ccsPortLineEdit->text();
	if (ccsPort.isEmpty())
	{
		QMessageBox::about(this, tr("login_dialog_title"), tr("ccs_port_request"));
		return;
	}
	if (!StringUtils::IsDigitStr(ccsPort))
	{
		QMessageBox::about(this, tr("login_dialog_title"), tr("ccs_port_error"));
		return;
	}
	QString rtsp = ui.rtspLineEdit->text();
	if (rtsp.isEmpty())
	{
		QMessageBox::about(this, tr("login_dialog_title"), tr("rtsp_address_request"));
		return;
	}
	QString account = ui.accountLineEdit->text();
	if (account.isEmpty())
	{
		QMessageBox::about(this, tr("login_dialog_title"), tr("account_request"));
		return;
	}
	PreferenceUtils::Store(KEY_CCS_SERVER, ccs.toStdString());
	PreferenceUtils::Store(KEY_CCS_PORT, ccsPort.toInt());
	PreferenceUtils::Store(KEY_RTSP_URL, rtsp.toStdString());
	PreferenceUtils::Store(KEY_ACCOUNT, account.toStdString());*/

	QSettings configIni(m_strInitPath, QSettings::IniFormat);
	configIni.setValue("rtsp/url", ccs);
	if (ui.rbTcpTransport->isChecked())
	{
		configIni.setValue("rtsp/transport", "tcp");
	}
	else 
	{
		configIni.setValue("rtsp/transport", "udp");
	}
	if (ui.rbGdiRender->isChecked())
	{
		configIni.setValue("video/render", "gdi");
	}
	else
	{
		configIni.setValue("video/render", "image");
	}
	QDialog::accept();
}
QString QtSettingDialog::GetRtspUrl()
{
	return ui.rtspUrlEdit->text().trimmed();
}
bool QtSettingDialog::IsTcpTransport()
{
	return ui.rbTcpTransport->isChecked();
}
QString QtSettingDialog::GetVideoRender()
{
	if (ui.rbGdiRender->isChecked())
	{
		return "gdi";
	}
	else
	{
		return "image";
	}
}