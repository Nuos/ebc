#include "AVCodecProxy.h"

#ifdef WIN32
#pragma comment(lib, "avcodec.lib") 
//#pragma comment(lib, "avdevice.lib") 
#pragma comment(lib, "avformat.lib") 
#pragma comment(lib, "avutil.lib") 
#pragma comment(lib, "swscale.lib") 

#include <windows.h>
#endif

//#ifdef USES_FFMPEG_3_3_2
//#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
//#endif

static int bit_buffer_size= 1024*256;
CAVCodecProxy::CAVCodecProxy(void)
: pFormatCtx(NULL)
, m_pVideoCodecCtxEnc(NULL)
, pVideoCodecCtx(NULL)
, pAudioCodecCtx(NULL)
, pVideoCodec(NULL)
, pAudioCodec(NULL)
, videoStream(-1)
, audioStream(-1)
, m_handler(NULL)
, m_pBufferDec(NULL)
, m_pBufferEnc(NULL)
, m_pFrameRGB(NULL)
, m_pFrameDecode(NULL)
, m_pFrameEncode(NULL)
, m_swsContextDecode(NULL)
, m_swsContextEncode(NULL)
, m_bFindKeyFrame(false)
, m_pBufferTemp(NULL), m_nBufferTempSize(0)
{
#ifdef USES_FFMPEG_3_3_2
	m_frame = av_frame_alloc();
#else
	m_samples = (unsigned char*)av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE * 2);
#endif
	m_bit_buffer = (unsigned char*)av_malloc(bit_buffer_size);
	av_init_packet(&avPacket);

}

CAVCodecProxy::~CAVCodecProxy(void)
{
	closeav();
#ifdef USES_FFMPEG_3_3_2
	av_frame_free(&m_frame);
#else
	av_free(m_samples);
#endif
	av_free(m_bit_buffer);
}

//��ȡ��һ֡
bool CAVCodecProxy::GetNextFrame(AVFormatContext *pFormatCtx, 
										AVCodecContext *pCodecCtx,
										int videoStream, 
										AVFrame *pFrame)
{
	static AVPacket packet;                    //AV������̬������
	static int      bytesRemaining=0;          //�ֽ�ʣ�ࡣ��̬������
	static bool     fFirstTime=true;           //��־����һ�Σ�����̬������
	int             bytesDecoded;              //������õ��ֽڣ�
	int             frameFinished;             //֡������ϱ�־��

	// First time we're called, set packet.data to NULL to indicate it
	// doesn't have to be freed ����һ�α����õ�ʱ�򣬰�packet.data����ΪNULL,�Ա�ʾ
	//��û�б�Ҫ���ͷţ�
	if (fFirstTime){
		fFirstTime = false;
		packet.data = NULL;
	}

	// Decode packets until we have decoded a complete frame
	while (true)
	{
		//Work on the current packet until we have decoded all of it
		while (bytesRemaining > 0)
		{
			// Decode the next chunk of data
			//bytesDecoded = avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);
//#ifdef USES_FFMPEG_3_3_2
			//avcodec_receive_frame
			bytesDecoded = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			

			// Was there an error?
			if (bytesDecoded < 0){
				fprintf(stderr, "Error while decoding frame\n");
				return false;
			}

			bytesRemaining -= bytesDecoded;
		//	rawData += bytesDecoded;

			// Did we finish the current frame? Then we can return
			if (frameFinished)
				return true;
		}

		// Read the next packet, skipping all packets that aren't for this
		// stream
		do{
			// Free old packet  �ͷžɰ�
			if(packet.data != NULL) {
#ifdef USES_FFMPEG_3_3_2
				av_packet_unref(&packet);
#else
				av_free_packet(&packet);
#endif
			}

			// Read new packet  ��ȡ�°�
			if(av_read_frame(pFormatCtx, &packet) < 0)
				goto loop_exit;
		} while(packet.stream_index != videoStream);  //������Ҫ�ҵ���Ƶ����ʱ�򣬼���ѭ�����������¶��ˣ�
		//ֱ���ҵ�Ҫ�ҵ���Ƶ�����˳�ѭ����

		bytesRemaining = packet.size; //��¼�����ֽ�����
	}

loop_exit:

	// Decode the rest of the last frame
	//bytesDecoded = avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, rawData, bytesRemaining);
	bytesDecoded = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
	//bytesDecoded = avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);

	// Free last packet
	if(packet.data != NULL) {
#ifdef USES_FFMPEG_3_3_2
		av_packet_unref(&packet);
#else
		av_free_packet(&packet);
#endif
	}

	return frameFinished != 0;
}


void CAVCodecProxy::closeav(void)
{
	//calculate decode rate �����������
//	t = clock() - t;
//	fps = (double)(t) / CLOCKS_PER_SEC;
//	fps = i_frame / fps;
//	printf("\n==>Decode rate %.4f fps!\n", fps);

	// Free the YUV image  �ͷ�yuvͼ��
	m_bFindKeyFrame = false;

	if (m_pBufferDec)
	{
		delete[] m_pBufferDec;
		m_pBufferDec = NULL;
	}
	if (m_pBufferEnc)
	{
		delete[] m_pBufferEnc;
		m_pBufferEnc = 0;
	}
	if (m_pBufferTemp)
	{
		delete[] m_pBufferTemp;
		m_pBufferTemp = NULL;
		m_nBufferTempSize = 0;
	}
	if (m_pFrameRGB)
	{
#ifdef USES_FFMPEG_3_3_2
		av_frame_free(&m_pFrameRGB);
#else
		av_free(m_pFrameRGB);
#endif
		m_pFrameRGB = NULL;
	}

	// Free the YUV frame  �ͷ�yuv֡
	if (m_pFrameDecode)
	{
#ifdef USES_FFMPEG_3_3_2
		av_frame_free(&m_pFrameDecode);
#else
		av_free(m_pFrameDecode);
#endif
		m_pFrameDecode = NULL;
	}
	if (m_pFrameEncode)
	{
#ifdef USES_FFMPEG_3_3_2
		av_frame_free(&m_pFrameEncode);
#else
		av_free(m_pFrameEncode);
#endif
		m_pFrameEncode = NULL;
	}
	// Close the codec
	if (pAudioCodecCtx)
	{
		avcodec_close(pAudioCodecCtx);
		pAudioCodecCtx = NULL;
	}
	if (pVideoCodecCtx)
	{
		avcodec_close(pVideoCodecCtx);
		pVideoCodecCtx = NULL;
	}
	if (m_pVideoCodecCtxEnc)
	{
		avcodec_close(m_pVideoCodecCtxEnc);
		m_pVideoCodecCtxEnc = NULL;
	}
	// Close the video file
	if (pFormatCtx)
	{
#ifdef USES_FFMPEG_3_3_2
		avformat_close_input(&pFormatCtx);
#else
		av_close_input_file(pFormatCtx);
#endif
		pFormatCtx = NULL;
	}

	if (m_swsContextDecode)
	{
		sws_freeContext(m_swsContextDecode);
		m_swsContextDecode = NULL;
	}
	if (m_swsContextEncode)
	{
		sws_freeContext(m_swsContextEncode);
		m_swsContextEncode = NULL;
	}
}

//����һ��bmp�ļ������ڵ���
static int av_create_bmp(char* filename,uint8_t *pRGBBuffer,
						 int width,int height,int bpp)
{
	BITMAPFILEHEADER bmpheader;
	BITMAPINFO bmpinfo;
	FILE *fp;

	fp = fopen(filename,"wb");
	if(!fp)return -1;

	bmpheader.bfType = ('M'<<8)|'B';
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;

	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = width;
	bmpinfo.bmiHeader.biHeight = height;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = bpp;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = 0;
	bmpinfo.bmiHeader.biXPelsPerMeter = 100;
	bmpinfo.bmiHeader.biYPelsPerMeter = 100;
	bmpinfo.bmiHeader.biClrUsed = 0;
	bmpinfo.bmiHeader.biClrImportant = 0;

	fwrite(&bmpheader,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&bmpinfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
	fwrite(pRGBBuffer,width*height*bpp/8,1,fp);
	fclose(fp);

	return 0;
}

long CAVCodecProxy::GetNextFrame(unsigned char *lpdata)
{

	// Read frames  ��ȡ����֡
	if (GetNextFrame(pFormatCtx, pVideoCodecCtx, videoStream, m_pFrameDecode))
	{
		//		img_convert((AVPicture *)m_pFrameRGB, DEST_FORMAT, (AVPicture*)pFrame, 
		//			pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
		SwsContext *ctx = sws_getContext(
			pVideoCodecCtx->width,
			pVideoCodecCtx->height,
			pVideoCodecCtx->pix_fmt,
			pVideoCodecCtx->width,
			pVideoCodecCtx->height,
#ifdef USES_FFMPEG_3_3_2
			AV_PIX_FMT_BGR24,
#else
			PIX_FMT_BGR24,
#endif
			SWS_BICUBIC, NULL, NULL, NULL);
		sws_scale(ctx,m_pFrameDecode->data,m_pFrameDecode->linesize,0,pVideoCodecCtx->height,m_pFrameRGB->data,m_pFrameRGB->linesize);

		//�����ã���C��д��һ��bmp�ļ���
		av_create_bmp("D:/WorkRecord/Project/1.bmp",(unsigned char *)m_pFrameRGB->data[0],pVideoCodecCtx->width,pVideoCodecCtx->height,32);

/*		i_frame++;
		y_size = pCodecCtx->width * pCodecCtx->height;                          

		//д���ļ�
		/*fwrite(m_pFrameRGB->data[0], 1, y_size, fp);
		fwrite(m_pFrameRGB->data[1], 1, (y_size/4), fp);
		fwrite(m_pFrameRGB->data[2], 1, (y_size/4), fp);*/
		//	memcpy(lpdata,m_pFrameRGB->data[0],y_size*3);
		//	UpendBmp(lpdata,pCodecCtx->width,pCodecCtx->height);
		return 0;
	}
	else
	{return -1;}
}

#ifdef USES_FFMPEG_3_3_2
bool CAVCodecProxy::openav(enum AVCodecID codecID, const unsigned char * extradata, int extrasize)
#else
bool CAVCodecProxy::openav(enum CodecID codecID, const unsigned char * extradata, int extrasize)
#endif
{
	if (isopenav()) return true;

	// Register all formats and codecs
	avcodec_register_all();
	//avdevice_register_all();
	av_register_all();

	// Find the decoder for the video stream
	AVCodec * pCodec = avcodec_find_decoder(codecID); 
	if (pCodec == NULL)
		return false; // Codec not found

#ifdef USES_FFMPEG_3_3_2
	AVCodecContext * pCodecCtx = avcodec_alloc_context3(0);
#else
	AVCodecContext * pCodecCtx = avcodec_alloc_context();
#endif
	if (pCodecCtx==0) {
		return false;
	}

	if (extrasize > 0)
	{
		pCodecCtx->extradata_size = extrasize;
		pCodecCtx->extradata = new unsigned char[extrasize+FF_INPUT_BUFFER_PADDING_SIZE];
		memset(pCodecCtx->extradata, 0, extrasize+FF_INPUT_BUFFER_PADDING_SIZE);
		memcpy(pCodecCtx->extradata, extradata, extrasize);
	}

	if (pCodec->type == AVMEDIA_TYPE_AUDIO)
	//if (pCodec->type == CODEC_TYPE_AUDIO)
	{
		pAudioCodecCtx = pCodecCtx;
		pAudioCodec = pCodec;
	}else if (pCodec->type == AVMEDIA_TYPE_VIDEO)
	//}else if (pCodec->type == CODEC_TYPE_VIDEO)
	{
		// some codecs need / can use extradata like Huffman tables.
		pVideoCodecCtx = pCodecCtx;
		pVideoCodec = pCodec;

		// ??
		pVideoCodecCtx->width = m_decFvinfo.width();
		pVideoCodecCtx->height = m_decFvinfo.height();

		//// ??
		//m_pVideoCodecCtxEnc = avcodec_alloc_context();
		//m_pVideoCodecCtxEnc->bit_rate = 400000;
		//m_pVideoCodecCtxEnc->width = 320;
		//m_pVideoCodecCtxEnc->height = 240;
		//m_pVideoCodecCtxEnc->sample_aspect_ratio.num = 25;
		//m_pVideoCodecCtxEnc->sample_aspect_ratio.den = 1;
		//m_pVideoCodecCtxEnc->time_base.num = 25;
		//m_pVideoCodecCtxEnc->time_base.den = 1;
		//m_pVideoCodecCtxEnc->pix_fmt = PIX_FMT_YUV420P;
		//m_pVideoCodecCtxEnc->gop_size = 240;
		//// OK
		//m_pVideoCodecCtxEnc->qmin = 10;
		//m_pVideoCodecCtxEnc->qmax = 51;
		//m_pVideoCodecCtxEnc->max_qdiff = 4;
		//m_pVideoCodecCtxEnc->me_range = 16;
		//m_pVideoCodecCtxEnc->keyint_min = 25;
		//m_pVideoCodecCtxEnc->qcompress = 0.6;
		//m_pVideoCodecCtxEnc->me_method = ME_FULL;

		////m_pVideoCodecCtxEnc->max_b_frames = 2;
		////if(!strcmp(pFormatCtxEnc->oformat->name, "mp4") || !strcmp(pFormatCtxEnc->oformat->name, "mov") || !strcmp(pFormatCtxEnc->oformat->name, "3gp"))
		//	//pVideoCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

		//if ( pCodec->capabilities & CODEC_CAP_TRUNCATED )
		//	m_pVideoCodecCtxEnc->flags|=CODEC_FLAG_TRUNCATED;

		//// Open codec //�򿪽�����
		//if ( avcodec_open(m_pVideoCodecCtxEnc, pCodec) < 0 )
		//	return false; // Could not open codec ���ܴ򿪽�����������-1��

	}

	// ??
	// Inform the codec that we can handle truncated bitstreams -- i.e.,    
	// bitstreams where frame boundaries can fall in the middle of packets 
	if ( pCodec->capabilities & CODEC_CAP_TRUNCATED )
		pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;


	// Open codec //�򿪽�����
#ifdef USES_FFMPEG_3_3_2
	AVDictionary *d = NULL;
	//av_dict_set
	if ( avcodec_open2(pCodecCtx, pCodec, &d) < 0 )
#else
	if ( avcodec_open(pCodecCtx, pCodec) < 0 )
#endif
		return false; // Could not open codec ���ܴ򿪽�����������-1��

	if (pCodec == pVideoCodec)
	{
		// Allocate video frame   ������Ƶ֡
#ifdef USES_FFMPEG_3_3_2
		m_pFrameDecode = av_frame_alloc();
#else
		m_pFrameDecode = avcodec_alloc_frame();
#endif
		if(m_pFrameDecode == NULL)
			return false;        

#ifdef USES_FFMPEG_3_3_2
		m_pFrameEncode = av_frame_alloc();
#else
		m_pFrameEncode = avcodec_alloc_frame();
#endif
		if(m_pFrameEncode == NULL)
			return false;        

		// Allocate an AVFrame structure     ����һ��AVFrame�ṹ
#ifdef USES_FFMPEG_3_3_2
		m_pFrameRGB=av_frame_alloc();     //������֡
#else
		m_pFrameRGB=avcodec_alloc_frame();     //������֡
#endif
		if(m_pFrameRGB == NULL)
			return false;        
	}

	return true;
}

bool CAVCodecProxy::isopenav(void) const
{
	return (pVideoCodecCtx != NULL || pAudioCodecCtx != NULL);
}

unsigned char* CAVCodecProxy::decode(const unsigned char * inBuffer, int inLength, unsigned char * outBuffer, int & outSize, bool reversal)
{
	int      bytesRemaining = inLength;		//�ֽ�ʣ�ࡣ��̬������
	int      bytesDecoded;					//������õ��ֽڣ�
	int      frameFinished=0;				//֡������ϱ�־��

//	bytesRemaining += (4-bytesRemaining % 4);
//	inLength = bytesRemaining;

	av_init_packet(&avPacket);
	//try
	//{
		if (m_pBufferTemp == NULL)
		{
			m_nBufferTempSize = inLength+FF_INPUT_BUFFER_PADDING_SIZE+1;
			m_pBufferTemp = new unsigned char[m_nBufferTempSize];
		}else if (m_nBufferTempSize < inLength+FF_INPUT_BUFFER_PADDING_SIZE+1)
		{
			delete[] m_pBufferTemp;
			m_nBufferTempSize = inLength+FF_INPUT_BUFFER_PADDING_SIZE+1;
			m_pBufferTemp = new unsigned char[m_nBufferTempSize];
		}
	//}

	memset(m_pBufferTemp, 0, m_nBufferTempSize);
	memcpy(m_pBufferTemp, inBuffer, inLength);
	avPacket.data = m_pBufferTemp;
	//avPacket.data = const_cast<unsigned char*>(inBuffer);
	avPacket.size = inLength;

	//Work on the current packet until we have decoded all of it
	while (bytesRemaining >= 0)
	{
		//FF_INPUT_BUFFER_PADDING_SIZE
		
		// Decode the next chunk of data
		if (pVideoCodecCtx)
		{
			//frameFinished = 1;
			bytesDecoded = avcodec_decode_video2(pVideoCodecCtx, m_pFrameDecode, &frameFinished, &avPacket);
			//bytesDecoded = avcodec_decode_video(pVideoCodecCtx, m_pFrameDecode, &frameFinished, inBuffer, inLength);
		}else if (pAudioCodecCtx)
		{
#ifdef USES_FFMPEG_3_3_2
			bytesDecoded = avcodec_decode_audio4(pAudioCodecCtx, m_frame, &frameFinished, &avPacket);
#else
			frameFinished = AVCODEC_MAX_AUDIO_FRAME_SIZE * 2;
			bytesDecoded = avcodec_decode_audio3(pAudioCodecCtx, (int16_t *)m_samples, &frameFinished, &avPacket);
#endif
			//bytesDecoded = avcodec_decode_audio2(pAudioCodecCtx, (int16_t *)m_samples, &frameFinished, inBuffer, inLength);
		}else
		{
			return NULL;
		}

		// Was there an error?
		if (bytesDecoded < 0){
			fprintf(stderr, "Error while decoding frame\n");

			avPacket.data = NULL;
			avPacket.size = 0;
			return NULL;
		}

		bytesRemaining -= bytesDecoded;

		// Did we finish the current frame? Then we can return
		if (frameFinished)
		{
			avPacket.data = NULL;
			avPacket.size = 0;

			//delete[] pBufferTemp;

			if (pAudioCodecCtx)
			{
				outSize = frameFinished;
				if (outBuffer==NULL)
					outBuffer = new unsigned char[outSize];
#ifdef USES_FFMPEG_3_3_2
				memcpy(outBuffer, m_frame->data[0], outSize);
#else
				memcpy(outBuffer, m_samples, outSize);
#endif
			}else if (pVideoCodecCtx)
			{
				if (!m_bFindKeyFrame)
				{
					m_bFindKeyFrame = m_pFrameDecode->key_frame == 1;
					if (!m_bFindKeyFrame)
						return NULL;
				}
				//m_pFrameEncode->data[0] = m_pFrameDecode->data[0];
				//m_pFrameEncode->data[1] = m_pFrameDecode->data[1];
				//m_pFrameEncode->data[2] = m_pFrameDecode->data[2];
				//m_pFrameEncode->linesize[0] = m_pFrameDecode->linesize[0];
				//m_pFrameEncode->linesize[1] = m_pFrameDecode->linesize[1];
				//m_pFrameEncode->linesize[2] = m_pFrameDecode->linesize[2];
				//
				//// For test
				//int bytesEncoded = avcodec_encode_video(m_pVideoCodecCtxEnc, (uint8_t *)m_bit_buffer, bit_buffer_size/2, m_pFrameEncode);

				if (m_pBufferDec == NULL)
				{
					// Determine required buffer size and allocate buffer   
					m_pBufferDec = new uint8_t [m_decFvinfo.framesize()];
#ifdef USES_FFMPEG_3_3_2
					av_image_fill_arrays(m_pFrameRGB->data,m_pFrameRGB->linesize,m_pBufferDec,m_decFvinfo.csp(), m_decFvinfo.width(), m_decFvinfo.height(),1);
#else
					//int numBytes = m_decFvinfo.framesize();
					avpicture_fill((AVPicture *)m_pFrameRGB, m_pBufferDec, m_decFvinfo.csp(), m_decFvinfo.width(), m_decFvinfo.height());
#endif
				}

				// For test.
				if (m_swsContextDecode == NULL)
				{
					m_swsContextDecode = sws_getContext(
						pVideoCodecCtx->width,
						pVideoCodecCtx->height,
						pVideoCodecCtx->pix_fmt,
						m_decFvinfo.width(),
						m_decFvinfo.height(),
						m_decFvinfo.csp(),
						SWS_BICUBIC,
						//SWS_POINT,
						NULL, NULL, NULL);
				}

				///////////////////////////////////////////////////
				unsigned char *data[4];
				int linesize[4];
				int i;

				// �ƶ�����ָ�룬ʵ��ͼ��ת��
				if (reversal)
				{
					// �ȼ�¼����ָ��
					for(i = 0; i < 4; i++)
					{
						data[ i ] = m_pFrameDecode->data[ i ];
						linesize[ i ] = m_pFrameDecode->linesize[ i ];
					}

					int height = pVideoCodecCtx->height;
					m_pFrameDecode->data[0] += m_pFrameDecode->linesize[0] * (height - 1);
					m_pFrameDecode->linesize[0] *= -1;                      
					m_pFrameDecode->data[1] += m_pFrameDecode->linesize[1] * (height / 2 - 1);
					m_pFrameDecode->linesize[1] *= -1;
					m_pFrameDecode->data[2] += m_pFrameDecode->linesize[2] * (height / 2 - 1);
					m_pFrameDecode->linesize[2] *= -1;
				}

				// x->RGB32 test
				sws_scale(m_swsContextDecode,m_pFrameDecode->data,m_pFrameDecode->linesize,0,pVideoCodecCtx->height,m_pFrameRGB->data,m_pFrameRGB->linesize);

				// �ָ�����ָ�룬���ⷢ���ڴ�й©��
				if (reversal)
				{
					for(i = 0; i < 4; i++)
					{
						m_pFrameDecode->data[ i ] = data[ i ];
						m_pFrameDecode->linesize[ i ] = linesize[ i ]; 
					}
				}

				//outSize = pVideoCodecCtx->width*pVideoCodecCtx->height*DEFAULT_BPP;
				outSize = m_decFvinfo.framesize();
				if (outBuffer==NULL)
					outBuffer = new unsigned char[outSize];
				memcpy(outBuffer, m_pFrameRGB->data[0], outSize);
				//�����ã���Ӳ��д��һ��bmp�ļ���
				//av_create_bmp("D:/Temp/2.bmp",(unsigned char *)m_pFrameRGB->data[0],pVideoCodecCtx->width,pVideoCodecCtx->height,24);
			}

			return outBuffer;
		}
	}
	avPacket.data = NULL;
	avPacket.size = 0;
	//delete[] pBufferTemp;
	return NULL;
}


bool CAVCodecProxy::encode(const unsigned char * inBuffer, int inLength, unsigned char ** outBuffer, int & outSize)
{
	int      bytesEncoded=0;					// ������õ��ֽڣ�

	if (pVideoCodecCtx)
	{
		if (m_pBufferEnc==0) {
#ifdef USES_FFMPEG_3_3_2
			const int numBytes = av_image_get_buffer_size(m_pVideoCodecCtxEnc->pix_fmt, m_pVideoCodecCtxEnc->width, m_pVideoCodecCtxEnc->height,1);
#else
			const int numBytes = avpicture_get_size(m_pVideoCodecCtxEnc->pix_fmt, m_pVideoCodecCtxEnc->width, m_pVideoCodecCtxEnc->height);
#endif
			m_pBufferEnc = new uint8_t[numBytes];
#ifdef USES_FFMPEG_3_3_2
			av_image_fill_arrays(m_pFrameEncode->data,m_pFrameEncode->linesize,m_pBufferEnc,m_pVideoCodecCtxEnc->pix_fmt, m_pVideoCodecCtxEnc->width, m_pVideoCodecCtxEnc->height,1);
#else
			avpicture_fill((AVPicture *)m_pFrameEncode, m_pBufferEnc, m_pVideoCodecCtxEnc->pix_fmt, m_pVideoCodecCtxEnc->width, m_pVideoCodecCtxEnc->height);
#endif
		}

		if (m_swsContextEncode == NULL)
		{
			m_swsContextEncode = sws_getContext(
				m_pVideoCodecCtxEnc->width,
				m_pVideoCodecCtxEnc->height,
#ifdef USES_FFMPEG_3_3_2
				AV_PIX_FMT_YUYV422,
#else
				PIX_FMT_YUYV422,
#endif
				m_pVideoCodecCtxEnc->width,
				m_pVideoCodecCtxEnc->height,
				m_pVideoCodecCtxEnc->pix_fmt,
				SWS_BICUBIC, NULL, NULL, NULL);
		}

#ifdef USES_FFMPEG_3_3_2
		AVFrame * big_picture = av_frame_alloc();
#else
		AVFrame * big_picture = avcodec_alloc_frame();
#endif
		big_picture->data[0] = const_cast<unsigned char*>(inBuffer);
		big_picture->linesize[0] = 320;

		sws_scale(m_swsContextEncode,big_picture->data,big_picture->linesize,0,m_pVideoCodecCtxEnc->height,m_pFrameEncode->data,m_pFrameEncode->linesize);

		//pVideoCodecCtx->pix_fmt = PIX_FMT_YUYV422;
		////m_pFrameEncode->data[0] = const_cast<unsigned char*>(inBuffer);
		//if (m_pFrameEncode->data[0] == NULL)
		//	m_pFrameEncode->data[0] = (unsigned char*)malloc(320*240*4);
		//memcpy(m_pFrameEncode->data[0], inBuffer, inLength);
		//m_pFrameEncode->linesize[0] = 320;

		// CODEC_FLAG_TRUNCATED
		//bytesEncoded = avcodec_encode_video(m_pVideoCodecCtxEnc, m_bit_buffer, bit_buffer_size, m_pFrameEncode);
#ifdef USES_FFMPEG_3_3_2
		av_frame_free(&big_picture);
#else
		av_free(big_picture);
#endif

	}else if (pAudioCodecCtx)
	{
		/*pAudioCodecCtx->frame_size = pAudioCodecCtx->bit_rate;
		uint8_t * samples = (uint8_t*)av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE*2);
		memcpy(samples, inBuffer, inLength); 
		bytesEncoded = avcodec_encode_audio(pAudioCodecCtx, m_bit_buffer, inLength, (const short*)samples);
		av_free(samples);*/

	}else
		return false;

	// Was there an error?
	if (bytesEncoded <= 0){
		fprintf(stderr, "Error while decoding frame\n");
		return false;
	}

	outSize = bytesEncoded;
	*outBuffer = new unsigned char[outSize];
	memcpy(*outBuffer, m_bit_buffer, outSize);
	memset(m_bit_buffer, 0, bit_buffer_size);

	return true;
}
