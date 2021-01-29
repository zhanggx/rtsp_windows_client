#pragma once

#include <QDialog>
#include "ui_QtSettingDialog.h"

class QtSettingDialog : public QDialog
{
	Q_OBJECT

public:
	QtSettingDialog(QWidget *parent = Q_NULLPTR);
	~QtSettingDialog();

public:
	QString GetRtspUrl();
	QString GetVideoRender();
	bool IsTcpTransport();
private:
	Ui::QtSettingDialog ui;

	QString m_strInitPath;
public slots:
	void OnConfirmButtonClicked();
};
