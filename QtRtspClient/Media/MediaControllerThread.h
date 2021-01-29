#pragma once
#include <QThread>

#include "MediaController.h"

typedef int (*FnRunThread)(MediaController* pController, int type,void* pParam);

class MediaControllerThread :public QThread
{
	Q_OBJECT
public:
	MediaControllerThread(MediaController* pController, FnRunThread fun, int type, void* pParam) :m_pController(pController), m_fun(fun), m_type(type), m_pParam(pParam)
	{
		connect(this, &QThread::finished, this, &QObject::deleteLater);
	}
	void run()
	{
		int result = m_fun(m_pController, m_type,m_pParam);
		emit OnThreadEnd(this, m_type, result);//复杂函数结束后发出信号
	}
private:
	MediaController* m_pController;
	FnRunThread m_fun;
	void* m_pParam;
	int m_type;
signals:
	void OnThreadEnd(const void* thread, const int type, const int result);

};
