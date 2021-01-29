#include "MediaDataStream.h"
#include "windows.h"
#include "Log.h"

MediaDataStream::MediaDataStream(const char* url, bool tcpTransport, MediaStreamDataHandler* pHandler):mUrl(url), m_bTcpTransport(tcpTransport),m_pHandler(pHandler)
{
	m_bRunning=false;
}
MediaDataStream::~MediaDataStream()
{
	
}
int AVInterruptCallBackFun(void* param)
{
	MediaDataStream* pMediaDataStream = (MediaDataStream*)param;
	if (NULL == pMediaDataStream) return 0;

	if (pMediaDataStream->IsRunning())
	{
		//通知FFMpeg继续阻塞工作
		return 0;
	}
	else
	{
		//通知FFMpeg可以从阻塞工作线程中释放操作
		return 1;
	}
}

void* MediaDataStream::OpenRtsp()
{
	AVDictionary* options = NULL;
	const char* in_filename = mUrl.c_str();
	AVFormatContext* pFormatContext = avformat_alloc_context();
	pFormatContext->flags |= AVFMT_FLAG_KEEP_SIDE_DATA;
	pFormatContext->flags |= AVFMT_FLAG_NONBLOCK;
	av_dict_set(&options, "buffer_size", "1024000", 0);
	av_dict_set(&options, "max_delay", "500000", 0);
	av_dict_set(&options, "stimeout", "8000000", 0);  //设置超时断开连接时间，8秒
	if (m_bTcpTransport) {
		av_dict_set(&options, "rtsp_transport", "tcp", 0);  //以tcp方式打开
	}
	//	av_dict_set(&options, "rtcp_port", "5001", 0);
	//	av_dict_set(&options, "local_rtcpport", "5001", 0);
	pFormatContext->interrupt_callback.callback = AVInterruptCallBackFun;
	pFormatContext->interrupt_callback.opaque = this;//超时回调

	int ret = avformat_open_input(&pFormatContext, in_filename, NULL, &options);//lhrtestm_csdpname
	if (ret < 0)
	{
		//qDebug("MDS open input usr fail\n");
		avformat_free_context(pFormatContext);
		return NULL;
	}
	if ((ret = avformat_find_stream_info(pFormatContext, 0)) < 0) {
		//qDebug("Failed to retrieve input stream information");
		avformat_close_input(&pFormatContext);
		avformat_free_context(pFormatContext);
		return NULL;
	}
	return pFormatContext;
}
int MediaDataStream::Start()
{
	if (m_bRunning)
	{
		return 0;
	}
	m_bRunning=true;
	av_register_all();
	avformat_network_init();

	AVFormatContext* pFormatContext = NULL;
	while (!pFormatContext && m_bRunning) {
		AVFormatContext* pContext=static_cast<AVFormatContext*>(OpenRtsp());
		if (pContext) {
			pFormatContext = pContext;
			LOG_INFO("-------------  Success to connect the rtsp camera:%s   -------------------------", mUrl.c_str());
			break;
		}
		LOG_ERROR("-------------  We can't connect the rtsp camera:%s-------------------", mUrl.c_str());
		Sleep(2000);
	}
	if (!pFormatContext||!m_bRunning) {
		LOGD("-------------  pFormatContext=====   -------------------------");
		if (m_pHandler)
		{
			m_pHandler->OnCameraOpenFailed();
		}
		return 0;
	}
	LOGD("-------------  pFormatContext OK   -------------------------");
	int mVideoindex = 0, mAudioindex = 0;
	for (int i = 0; i < pFormatContext->nb_streams; i++) {
		if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			mVideoindex = i;
			if (!pFormatContext->streams[i]->codec->width) {//流通道没有宽高值
				LOGD("-------------  DataSources  video 没有宽值   -------------------------\n");
				//return FAIL;
			}
		}
		else if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			AVCodecContext* codec = pFormatContext->streams[i]->codec;
			/*if(mDataSourceIF){ //之前是探测成功就初始化收流  现在要先初始化发流
				AudioInfo info;
				info.channels = codec->channels;
				info.sameple_rate = codec->sample_rate;
				info.channel_layout = codec->channel_layout;
				info.bit_rate = codec->bit_rate;
				mDataSourceIF->OnDataSourceAudioInfo(info);
			}*/
			mAudioindex = i;
		}
	}
	char cameraURLStr[LITTLE_ARRAY_LEN];
	memset(cameraURLStr, 0, LITTLE_ARRAY_LEN);
	av_dump_format(pFormatContext, 0, cameraURLStr, 0);

	int ret = 0;

	AVPacket pkt;
	AVPicture pAVPicture;
	av_init_packet(&pkt);
	int frameCount = 0;
	//FILE* file = fopen(mLocalFileName.c_str(), "w");
	///查找解码器
	AVCodecContext* pVideoCodecCtx = pFormatContext->streams[mVideoindex]->codec;
	AVCodec* pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
	AVCodecContext* pAudioCodecCtx = pFormatContext->streams[mAudioindex]->codec;
	AVCodec* pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);

	SwsContext* pSwsContext = NULL;
	SwrContext* pAudioSwrContext = NULL;
	//视频分辨率
	int videoWidth = pVideoCodecCtx->width;
	int videoHeight = pVideoCodecCtx->height;
	if (pVideoCodec)
	{
		avpicture_alloc(&pAVPicture, AV_PIX_FMT_RGB24, videoWidth, videoHeight);

		pSwsContext = sws_getContext(videoWidth, videoHeight, AV_PIX_FMT_YUV420P, videoWidth, videoHeight, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, 0, 0, 0);

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
	AVSampleFormat audioSampleFormat = AV_SAMPLE_FMT_S32;
	int channels = pAudioCodecCtx->channels;//通道数
	int bytes_per_sample = av_get_bytes_per_sample(audioSampleFormat);
	int sampleSize = 8 * bytes_per_sample;
	int sampleRate = pAudioCodecCtx->sample_rate;

	LOG_DEBUG("input video done,frameCount=%d", frameCount);
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
			pAudioSwrContext = swr_alloc_set_opts(pAudioSwrContext,AV_CH_LAYOUT_STEREO, audioSampleFormat, sampleRate,
				pAudioCodecCtx->channel_layout, pAudioCodecCtx->sample_fmt, pAudioCodecCtx->sample_rate,0, NULL);
			swr_init(pAudioSwrContext);
			//if (m_pAudioPlayer)
			//{
			//	m_pAudioPlayer->Start(sampleRate, sampleSize, channels);
		//	}
		}
	}
	else
	{
		LOGW("---- Codec cann't found  ----\n");
	}
	int out_linesize;
	int out_buffer_size = av_samples_get_buffer_size(&out_linesize, channels, pAudioCodecCtx->frame_size, audioSampleFormat, 1);
	uint8_t* out_buffer = (uint8_t*)av_malloc(out_buffer_size);

	//emit mediaOpened(videoWidth, videoHeight);
	if (m_pHandler)
	{
		m_pHandler->OnMediaOpenReady(videoWidth, videoHeight,sampleRate,sampleSize,channels);
	}
	int audio_data_size = av_get_bytes_per_sample(pAudioCodecCtx->sample_fmt);
	AVFrame* pAVFrame = av_frame_alloc();
	//av_init_packet(&videoPkt)
	int got_picture;
	int byt = unsigned(pVideoCodecCtx->extradata[4] & 0x1f);
	LOG_DEBUG("input video done,frameCount=%d", frameCount);
	unsigned char tempData[500000];
	int errorCount = 0;
	while (m_bRunning) {
		int64_t mReadFrameTime = av_gettime();
		pkt.data = NULL;
		pkt.size = 0;
		pkt.stream_index = 0;
		ret = av_read_frame(pFormatContext, &pkt);
		if (ret < 0) {
			LOG_DEBUG(" -------   DataSources::RunFrame   cant't read packet!  ret=%d,errorCount=%d", ret, errorCount);
			if (errorCount < 3) {
				errorCount++;
				continue;
			}
			errorCount = 0;
			if (!m_bRunning) {
				break;
			}
			if (m_pHandler)
			{
				m_pHandler->OnCameraOpenFailed();
			}
			avpicture_free(&pAVPicture);
			sws_freeContext(pSwsContext);
			swr_free(&pAudioSwrContext);
			avcodec_close(pVideoCodecCtx);
			avcodec_close(pAudioCodecCtx);
			avformat_close_input(&pFormatContext);
			avformat_free_context(pFormatContext);
			pVideoCodecCtx = NULL;
			pAudioCodecCtx = NULL;
			pFormatContext = NULL;
			while (!pFormatContext && m_bRunning) {
				AVFormatContext* pContext = static_cast<AVFormatContext*>(OpenRtsp());
				if (pContext) {
					pFormatContext = pContext;
					LOG_INFO("-------------  Success to connect the rtsp camera:%s   -------------------------", mUrl.c_str());
					break;
				}
				LOG_ERROR("-------------  We can't connect the rtsp camera:%s-------------------", mUrl.c_str());
				Sleep(2000);
			}
			if (!pFormatContext || !m_bRunning) {
				LOGD("-------------  pFormatContext=====   -------------------------");
				break;
			}	///查找解码器
			pVideoCodecCtx = pFormatContext->streams[mVideoindex]->codec;
			pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
			pAudioCodecCtx = pFormatContext->streams[mAudioindex]->codec;
			pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);

			//视频分辨率
			int videoWidth = pVideoCodecCtx->width;
			int videoHeight = pVideoCodecCtx->height;
			if (pVideoCodec)
			{
				avpicture_alloc(&pAVPicture, AV_PIX_FMT_RGB24, videoWidth, videoHeight);

				pSwsContext = sws_getContext(videoWidth, videoHeight, AV_PIX_FMT_YUV420P, videoWidth, videoHeight, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, 0, 0, 0);

				//打开对应解码器
				int result = avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL);
				if (result < 0) {
					LOGD("打开解码器失败");
				}
			}
			else
			{
				LOGW("---- Codec can't found  ----");
			}
			audioSampleFormat = AV_SAMPLE_FMT_S32;
			channels = pAudioCodecCtx->channels;//通道数
			bytes_per_sample = av_get_bytes_per_sample(audioSampleFormat);
			sampleSize = 8 * bytes_per_sample;
			sampleRate = pAudioCodecCtx->sample_rate;

			LOG_DEBUG("input video done,frameCount=%d", frameCount);
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
					pAudioSwrContext = swr_alloc_set_opts(pAudioSwrContext,AV_CH_LAYOUT_STEREO, audioSampleFormat, sampleRate,
						pAudioCodecCtx->channel_layout, pAudioCodecCtx->sample_fmt, pAudioCodecCtx->sample_rate,0, NULL);
					swr_init(pAudioSwrContext);
					//if (m_pAudioPlayer)
					//{
					//	m_pAudioPlayer->Start(sampleRate, sampleSize, channels);
				//	}
				}
			}
			else
			{
				LOGW("---- Codec cann't found  ----");
			}
			if (m_pHandler)
			{
				m_pHandler->OnMediaOpenReady(videoWidth, videoHeight, sampleRate, sampleSize, channels);
			}
		}
		// LOGE(" -------   DataSources::RunFrame read packet! frameCount=%d", frameCount);

		if (pkt.data == NULL) {//海康的摄像头会发空包。
			LOGD("---- DataSources::RunFrame     pkt.data == NULL  ----");
			continue;
		}
		//LogManager::GetInstance()->WriteError(" -------   DataSources#RunFrame   pkt.size=%d pkt.data!=null mGetAudioVideoThread=%ld", pkt.size,mGetAudioVideoThread);
		if (!m_bRunning)
		{
			break;
		}
		if (pkt.stream_index == mVideoindex) {
			pkt.flags = 1;
			if (pkt.data) {
				pkt.dts = pkt.pts;
				// pkt.pts= m_vnum++ * (gcameraFormatCtx->streams[mVideoindex]->time_base.num * 1000 / gcameraFormatCtx->streams[mVideoindex]->time_base.den);
				//fwrite(pkt.data, pkt.size, 1, file);
				if (frameCount % 5 == 0) {
					//qDebug("input video pkt.size=%d,pkt.pts=%lld,frameCount=%d\n", pkt.size, pkt.pts, frameCount);
					//fflush(file);
				}
				//PutAudioVideoData(audioVideoJniEnv,audioVideoArray,reinterpret_cast<jbyte *>(pkt.data),pkt.size,pkt.pts,pkt.dts,VIDEO_DATA,STREAM_SOURCE_MASTER);
				int byt = unsigned(pkt.data[4] & 0x1f);
				//qDebug("s data=%d-%d-%d-%d-%d-%d-%d#byt=%d,len=%d", unsigned(pkt.data[0]), unsigned(pkt.data[1]), unsigned(pkt.data[2]), unsigned(pkt.data[3]),
				//	unsigned(pkt.data[4]), unsigned(pkt.data[5]), unsigned(pkt.data[6]), byt,pkt.size);
				if (m_pHandler)
				{
					m_pHandler->OnMediaPacketDataReady(pkt.data, pkt.size,pkt.pts, MEDIA_FRAME_TYPE_VIDEO);
				}
				if (pVideoCodec&& m_pHandler)//解码
				{
					avcodec_decode_video2(pVideoCodecCtx, pAVFrame, &got_picture, &pkt);
					if (got_picture) {
						//mutex.lock();
						sws_scale(pSwsContext, (const uint8_t* const*)pAVFrame->data, pAVFrame->linesize, 0, videoHeight, pAVPicture.data, pAVPicture.linesize);
						m_pHandler->OnMediaFrameDataReady(pAVPicture.data[0], pAVPicture.linesize[0], pkt.pts, MEDIA_FRAME_TYPE_VIDEO);
						//mutex.unlock();
					}
				}
			}
			/*if(m_camindex && m_stream_data)
			{
				m_stream_data->pop();
				pkt.data = cammainbuf;
				pkt.size = cammainlen;
			}*/
			av_packet_unref(&pkt);
			frameCount++;
			//  m_stream_data->pop();
		}
		else if (pkt.stream_index == mAudioindex) {

			pkt.flags = 1;
			if (pkt.data) {
				pkt.dts = pkt.pts;
				if (m_pHandler)
				{
					m_pHandler->OnMediaPacketDataReady(pkt.data, pkt.size, pkt.pts, MEDIA_FRAME_TYPE_AUDIO);
				}
				// pkt.dts = pkt.pts = m_anum++ * (1024 * 1000 / 48000);
				//LogManager::GetInstance()->WriteError("input audio pkt.pts=%d", pkt.pts);
				//audioVideoJniEnv->SetByteArrayRegion(audioVideoArray, 0,pkt.size,(jbyte*)pkt.data);
				//PutAudioVideoData(audioVideoJniEnv,audioVideoArray,reinterpret_cast<jbyte *>(pkt.data),pkt.size,pkt.pts,pkt.dts,AUDIO_DATA,STREAM_SOURCE_MASTER);
				//
				if (pAudioCodec && m_pHandler)
				{
					int len = avcodec_decode_audio4(pAudioCodecCtx, pAVFrame, &got_picture, &pkt);
					if (got_picture) {
						int size = static_cast<size_t>(pAVFrame->linesize[0]);
						swr_convert(pAudioSwrContext, &out_buffer, out_linesize, (const uint8_t**)pAVFrame->data, pAVFrame->nb_samples);
						m_pHandler->OnMediaFrameDataReady(out_buffer, out_linesize, pkt.pts, MEDIA_FRAME_TYPE_AUDIO);
						/*if (m_pAudioPlayer)
						{
							m_pAudioPlayer->Play((const char*)out_buffer, out_linesize);
							//m_pAudioPlayer->Play((const char*)pAVFrame->data[0], size);
						}*/
					}
				}
			}
			av_packet_unref(&pkt);
		}
		Sleep(0);
	}
	av_free(out_buffer);
	av_free_packet(&pkt);//释放资源,否则内存会一直上升
	LOG_DEBUG("input video done,frameCount=%d", frameCount);
	//fclose(file);
	av_frame_free(&pAVFrame);
	if (pFormatContext) {
		avpicture_free(&pAVPicture);
		sws_freeContext(pSwsContext);
		swr_free(&pAudioSwrContext);
		avcodec_close(pVideoCodecCtx);
		avcodec_close(pAudioCodecCtx);
		avformat_close_input(&pFormatContext);
		avformat_free_context(pFormatContext);
		pFormatContext = NULL;
	}

	return 1;
}
int MediaDataStream::Stop()
{
	m_bRunning = false;
	return 1;
}