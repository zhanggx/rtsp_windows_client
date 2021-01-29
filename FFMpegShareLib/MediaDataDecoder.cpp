#include "MediaDataDecoder.h"


#include "DataStream.h"
#include "Log.h"

MediaDataDecoder::MediaDataDecoder(MediaDataHandler* pHandler):m_pHandler(pHandler)
{
	m_bDestroyed = true;
	pSwsContext = NULL;
	pAudioSwsContext = NULL;
	pAudioSwrContext = NULL;
	videoWidth = 0;
	videoHeight = 0;
		out_buffer=NULL;
		out_linesize = 0;
		out_buffer_size = 0;
		
}
MediaDataDecoder::~MediaDataDecoder()
{
	
}
int MediaDataDecoder::Init(int nDstVideoWidth, int nDstVideoHeight)
{
	LOG_DEBUG("%s\n", avcodec_configuration());

	m_nFrameCount = 0;
	m_bDestroyed = false;
	m_nDstVideoWidth = nDstVideoWidth;
	m_nDstVideoHeight= nDstVideoHeight;
	av_register_all();

	av_init_packet(&pkt);
	//FILE* file = fopen(mLocalFileName.c_str(), "w");
	///查找解码器
	AVCodec* pVideoCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	pVideoCodecCtx = avcodec_alloc_context3(pVideoCodec);
	pVideoCodecCtx->frame_number = 1;
	pVideoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	// 视频的宽度和高度
	pVideoCodecCtx->width = nDstVideoWidth;
	pVideoCodecCtx->height = nDstVideoHeight;

	AVCodec* pAudioCodec = avcodec_find_decoder(AV_CODEC_ID_AAC);
	pAudioCodecCtx = avcodec_alloc_context3(pAudioCodec);
	AVSampleFormat audioSampleFormat = AV_SAMPLE_FMT_S32;
	int channels = 2;//通道数
	int sampleRate = 48000;
	int bytes_per_sample = av_get_bytes_per_sample(audioSampleFormat);
	int sampleSize = 8 * bytes_per_sample;
	pAudioCodecCtx->channels = channels;
	pAudioCodecCtx->sample_fmt = audioSampleFormat;
	pAudioCodecCtx->sample_rate = sampleRate;
	pAudioCodecCtx->frame_size = 1024;
	pAudioCodecCtx->channel_layout = 3;
	//AVPacket videoPkt;
	//视频分辨率
	if (pVideoCodec)
	{
		//打开对应解码器
		int result = avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL);
		if (result < 0) {
			LOGD("打开解码器失败");
		}
	}
	else
	{
		LOGW("---- Codec can't found  ----\n");
	}

	LOG_DEBUG("audio sample rate:%d sample size:%d chanle:%d\n", sampleRate, sampleSize, channels);

	if (pAudioCodec)
	{
		//打开对应解码器
		int result = avcodec_open2(pAudioCodecCtx, pAudioCodec, NULL);
		if (result < 0) {
			LOGD("打开解码器失败");
		}
		else
		{
			pAudioSwrContext = swr_alloc();
			pAudioSwrContext = swr_alloc_set_opts(pAudioSwrContext,
				AV_CH_LAYOUT_STEREO, audioSampleFormat, sampleRate,
				pAudioCodecCtx->channel_layout, pAudioCodecCtx->sample_fmt, pAudioCodecCtx->sample_rate,
				0, NULL);
			swr_init(pAudioSwrContext);
			//if (m_pAudioPlayer)
			//{
			//	m_pAudioPlayer->Start(sampleRate, sampleSize, channels);
			//}
		}
	}
	else
	{
		LOGW("---- Codec cann't found  ----\n");
	}
	avpicture_alloc(&pAVPicture, AV_PIX_FMT_RGB24, nDstVideoWidth, nDstVideoHeight);

	out_buffer_size = av_samples_get_buffer_size(&out_linesize, channels, pAudioCodecCtx->frame_size, audioSampleFormat, 1);
	out_buffer = (uint8_t*)av_malloc(out_buffer_size);
	pAVFrame = av_frame_alloc();
	m_pHandler->OnMediaOpenReady(nDstVideoWidth, m_nDstVideoHeight, sampleRate, sampleSize, channels);
	return 1;
}
int MediaDataDecoder::Decode(MediaData* pMediaData)
{
	if (m_bDestroyed)
	{
		return 0;
	}
	pkt.data = pMediaData->buf;
	pkt.size = pMediaData->len;
	pkt.stream_index = 0;
	pkt.pts = pMediaData->pts;
	pkt.dts = pMediaData->dts;
	pkt.duration = 40;
	pkt.pos = -1;
	pkt.flags = 1;

	//LogManager::GetInstance()->WriteError(" -------   DataSources#RunFrame   pkt.size=%d pkt.data!=null mGetAudioVideoThread=%ld", pkt.size,mGetAudioVideoThread);

	int got_picture;
	if (pMediaData->type == 0) {
		pkt.pts = av_rescale_q(pMediaData->pts, { 1,90000 }, { 1,1000 });
		pkt.dts = pMediaData->dts;
		avcodec_decode_video2(pVideoCodecCtx, pAVFrame, &got_picture, &pkt);
		if (got_picture) {
			if (pSwsContext == NULL || pAVFrame->width != videoWidth || pAVFrame->height != videoHeight) {
				if (pSwsContext)
				{
					avpicture_free(&pAVPicture);
					sws_freeContext(pSwsContext);
				}
				videoWidth = pAVFrame->width;
				videoHeight = pAVFrame->height;
				pSwsContext = sws_getContext(videoWidth, videoHeight, AV_PIX_FMT_YUV420P, m_nDstVideoWidth, m_nDstVideoHeight, AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
			}
			//mutex.lock();
			sws_scale(pSwsContext, (const uint8_t* const*)pAVFrame->data, pAVFrame->linesize, 0, videoHeight, pAVPicture.data, pAVPicture.linesize);
			//发送获取一帧图像信号
			//QImage image(pAVPicture.data[0], m_nDstVideoWidth, m_nDstVideoHeight, QImage::Format_RGB888);
			//emit videoImageReady(image);
			//mutex.unlock();
			m_pHandler->OnMediaFrameDataReady(pAVPicture.data[0], pAVPicture.linesize[0], pkt.pts, MEDIA_FRAME_TYPE_VIDEO);
		}
		m_nFrameCount++;
		//  m_stream_data->pop();
	}
	else
	{
		int len = avcodec_decode_audio4(pAudioCodecCtx, pAVFrame, &got_picture, &pkt);
		if (got_picture) {
			int size = static_cast<size_t>(pAVFrame->linesize[0]);
			//qDebug("---- avcodec_decode_audio4:size=%d----",size);
			swr_convert(pAudioSwrContext, &out_buffer, out_linesize, (const uint8_t**)pAVFrame->data, pAVFrame->nb_samples);
			//if (m_pAudioPlayer)
			//{
			//	m_pAudioPlayer->Play((const char*)out_buffer, out_linesize);
				//m_pAudioPlayer->Play((const char*)pAVFrame->data[0], size);
			//}
			m_pHandler->OnMediaFrameDataReady(out_buffer, out_linesize, pkt.pts, MEDIA_FRAME_TYPE_AUDIO);
		}
	}
	av_packet_unref(&pkt);
	return 1;
}
int MediaDataDecoder::Destory()
{
	av_free_packet(&pkt);//释放资源,否则内存会一直上升
	LOG_DEBUG("input video done,frameCount=%d", m_nFrameCount);
	//fclose(file);
	avpicture_free(&pAVPicture);
	av_frame_free(&pAVFrame);
	sws_freeContext(pSwsContext);
	avcodec_close(pAudioCodecCtx);
	avcodec_free_context(&pAudioCodecCtx);
	avcodec_close(pAudioCodecCtx);
	avcodec_free_context(&pAudioCodecCtx);
	return 1;
}