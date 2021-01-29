#include "AudioPlayer.h"

#include <cstddef>

AudioPlayer::AudioPlayer()
{
	m_audioOutput=NULL;
	m_ioDevice=NULL;
}
AudioPlayer::~AudioPlayer()
{
	Stop();
}
void AudioPlayer::Start(int sampleRate, int sampleSize, int channel)
{
	if (m_audioOutput)
	{
		Stop();
	}
	mutex.lock();
	QAudioFormat fmt;//������Ƶ�����ʽ
	fmt.setSampleRate(sampleRate);//1�����Ƶ������
	fmt.setSampleSize(sampleSize);//���������Ĵ�С
	fmt.setChannelCount(channel);//����
	fmt.setCodec("audio/pcm");//�����ʽ
	//fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::SignedInt);//������Ƶ����
	//fmt.setSampleType(QAudioFormat::Float);//������Ƶ����
	
	m_audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), fmt);
	
	m_ioDevice = m_audioOutput->start();//���ſ�ʼ
	
	mutex.unlock();
}
void AudioPlayer::Play(const char* data, int size)
{

	if (m_ioDevice)
	{
		mutex.lock();
		if (m_ioDevice)
			m_ioDevice->write(data, size);//����ȡ����Ƶд�뵽��������
		mutex.unlock();
	}
}
void AudioPlayer::Stop()
{
	if (m_audioOutput)//Ϊ��AudioOutput
	{
		mutex.lock();
		if (m_audioOutput)//Ϊ��AudioOutput
		{
			m_audioOutput->stop();
			delete m_audioOutput;
			m_audioOutput = NULL;
			m_ioDevice = NULL;
		}
		mutex.unlock();
	}
}