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
	QAudioFormat fmt;//设置音频输出格式
	fmt.setSampleRate(sampleRate);//1秒的音频采样率
	fmt.setSampleSize(sampleSize);//声音样本的大小
	fmt.setChannelCount(channel);//声道
	fmt.setCodec("audio/pcm");//解码格式
	//fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::SignedInt);//设置音频类型
	//fmt.setSampleType(QAudioFormat::Float);//设置音频类型
	
	m_audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), fmt);
	
	m_ioDevice = m_audioOutput->start();//播放开始
	
	mutex.unlock();
}
void AudioPlayer::Play(const char* data, int size)
{

	if (m_ioDevice)
	{
		mutex.lock();
		if (m_ioDevice)
			m_ioDevice->write(data, size);//将获取的音频写入到缓冲区中
		mutex.unlock();
	}
}
void AudioPlayer::Stop()
{
	if (m_audioOutput)//为打开AudioOutput
	{
		mutex.lock();
		if (m_audioOutput)//为打开AudioOutput
		{
			m_audioOutput->stop();
			delete m_audioOutput;
			m_audioOutput = NULL;
			m_ioDevice = NULL;
		}
		mutex.unlock();
	}
}