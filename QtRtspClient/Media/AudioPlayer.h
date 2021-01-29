#pragma once
#include <QtMultimedia/QAudioOutput>
#include <QIODevice>
#include <QMutex>


class AudioPlayer
{
public:
	AudioPlayer();
	~AudioPlayer();
public:
	void Start()
	{
		Start(default_sampleRate, default_sampleSize, default_channel);
	}
	void Start(int sampleRate, int sampleSize, int channel);
	void Play(const char* data, int size);
	void Stop();
private:
	const int default_sampleRate = 48000;//样本率
	const int default_sampleSize = 16;//样本大小
	const int default_channel = 2;///通道数
	QAudioOutput* m_audioOutput;
	QMutex mutex;
	QIODevice* m_ioDevice;
	QVector<QString> m_outputDeviceList;
};

