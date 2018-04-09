//��  

#include "Common.h"

#ifdef MYDECODER

#ifndef AUDIO_DECODER

#define __STDC_FORMAT_MACROS  
#ifdef _CPPRTTI   
extern "C"
{
#endif  
	#include "libavutil/imgutils.h"    //ͼ�񹤾�   
	#include "libavutil/samplefmt.h"  // ��Ƶ������ʽ  
	#include "libavutil/timestamp.h"  //ʱ������߿��� �����ڵ��Ժ���־Ŀ��   
	#include "libavcodec\avcodec.h"
	#include "libavformat/avformat.h" //Main libavformat public API header  ������libavf I/O��   Demuxing  ��Muxing ��     
	#include "SDL.h"  
#ifdef _CPPRTTI   
};
#endif  

//����Ƶ������������  
static AVCodecContext *pVideoContext, *pAudioContext;
static FILE *fVideoFile, *fAudioFile;  //����ļ����  
static AVStream *pStreamVideo, *pStreamAudio; //ý����    
static unsigned char * videoDstData[4];  //��Ƶ����   
static int videoLineSize[4]; //   
static int videoBufferSize; //��Ƶ��������С   
static AVFormatContext *pFormatCtx = NULL; //��ʽ������  
static AVFrame*pFrame = NULL; //  
static AVPacket pkt;  //����ý���  
static int ret = 0; //״̬  
static int gotFrame; //��ȡ������Ƶ�� 

AVCodec *pVideoDec = NULL;
AVCodec *pAudioDec = NULL;

					 //����Ƶ��������  
static int videoStreamIndex, audioStreamIndex;
//����ý���  
//SDL����    
SDL_Window * pWindow = NULL;
SDL_Renderer *pRender = NULL;
SDL_Texture *pTexture = NULL;
SDL_Rect dstrect = { 0,0,800,600 };
int frame = 0;
int indexFrameVideo = 0;
static int decode_packet(int* gotFrame, int param2)
{
	int ret = 0;
	//�������ݴ�С  
	int decodedSize = pkt.size;
	//��ʼ����ȡ������֡Ϊ0  
	*gotFrame = 0;
	//�������Ƶ����ô �����Ƶ��    
	if (pkt.stream_index == videoStreamIndex)
	{

		//�������ݵ���Ƶ֡  
		if ((ret = avcodec_decode_video2(pVideoContext, pFrame, gotFrame, &pkt))<0)
		{
			//������Ƶ֡ʧ��  
			return ret;
		}
		indexFrameVideo++;
		//copy ��ѹ������ݵ����Ƿ���Ŀռ���  
		if (*gotFrame)
		{
			//��������  
			av_image_copy(videoDstData, videoLineSize, (const uint8_t **)(pFrame->data), pFrame->linesize, pVideoContext->pix_fmt, pVideoContext->width, pVideoContext->height);
			//д�����ݵ�������  
			fwrite(videoDstData[0], 1, videoBufferSize, fVideoFile);  
			printf("�����ǰ��%d֡,��С:%d\n", indexFrameVideo, videoBufferSize);
			int n = SDL_BYTESPERPIXEL(pStreamVideo->codec->pix_fmt);

			//��������  

			SDL_UpdateTexture(pTexture, &dstrect, (const void*)videoDstData[0], videoLineSize[0]);

			//��������2Dģ��  
			SDL_RenderCopy(pRender, pTexture, NULL, &dstrect);
			//��ʱ 1000ms*1/25  
			SDL_Delay(1000 * 1 / frame);
			//��ʾRender��Ⱦ��  
			SDL_RenderPresent(pRender);
		}
		else
		{
			printf("��%d֡,��ʧ\n", indexFrameVideo);
		}
	}
	//��Ƶ����  
	else if (pkt.stream_index == audioStreamIndex)
	{
		//������Ƶ��Ϣ  
		if ((ret = avcodec_decode_audio4(pAudioContext, pFrame, gotFrame, &pkt)) < 0)  
		    return ret;  
		decodedSize = FFMIN(ret, pkt.size);  
		//�����ǰ֡�Ĵ�С  
		size_t unpadded_linesize = pFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)pFrame->format);   
		///д�����ݵ���Ƶ�ļ�  
		fwrite(pFrame->extended_data[0], 1, unpadded_linesize, fAudioFile);     
	}
	//ȡ����������  ��������frame�ֶ�  
	av_frame_unref(pFrame);
	return decodedSize;
}

int Demuxing(int argc, char** argv)
{
	if (argc < 4)
	{
		printf("Parameter Error!\n");
		return 0;
	}

	//ע�����л����� ������  
	av_register_all();
	//ע�����б�����  
	avcodec_register_all();
	//ý������Դͷ  
	char*pInputFile = argv[1];
	strcpy(pInputFile, argv[1]);
	//��Ƶ����ļ�  
	char*pOutputVideoFile = argv[2];
	//��Ƶ����ļ�  
	char*pOutputAudioFile = argv[3];
	//���价��������  
	pFormatCtx = avformat_alloc_context();
	//������Դ  ���Ҷ�ȡ����Դ��ͷ��  
	if (avformat_open_input(&pFormatCtx, pInputFile, NULL, NULL) < 0)
	{
		printf("Open Input Error!\n");
		return 0;
	}
	//��ȡ��ý����Ϣ  
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("��ȡ��ý����Ϣʧ��!\n");
		return 0;
	}
	//��ӡý����Ϣ  
	av_dump_format(pFormatCtx, 0, pInputFile, 0);
	for (unsigned i = 0; i < pFormatCtx->nb_streams; i++)
	{
		AVStream *pStream = pFormatCtx->streams[i];
		AVMediaType mediaType = pStream->codec->codec_type;
		//��ȡ��ͬ�ı������  
		if (mediaType == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamIndex = i;
			pVideoContext = pStream->codec;
			pStreamVideo = pStream;
			fVideoFile = fopen(pOutputVideoFile, "wb");
			frame = pVideoContext->framerate.num;
			if (!fVideoFile)
			{
				printf("con't open file!\n");
				goto end;
			}
			//��������һ֡ͼ��Ĵ�С  
			//int nsize = avpicture_get_size(PIX_FMT_YUV420P, 1280, 720);  
			//��������ʼ�� ͼ�񻺳��� ��ɫ������  
			int ret = av_image_alloc(videoDstData, videoLineSize, pVideoContext->width, pVideoContext->height, pVideoContext->pix_fmt, 1);
			if (ret < 0)
			{
				printf("Alloc video buffer error!\n");
				goto end;
			}
			//avpicture_fill((AVPicture *)pFrame, videoDstData[0], PIX_FMT_YUV420P, 1280, 720);  
			videoBufferSize = ret;

			//���ݱ�����id���ұ�����  
			pVideoDec = avcodec_find_decoder(pStream->codec->codec_id);
			if (pVideoDec == NULL)
			{
				printf("���ұ�����ʧ��!\n");
				goto end;
			}
			if (avcodec_open2(pStream->codec, pVideoDec, nullptr) != 0)
			{
				printf("�򿪱�����ʧ��!\n");
				goto end;
			}
		}
		else if (mediaType == AVMEDIA_TYPE_AUDIO)
		{
			audioStreamIndex = i;
			pAudioContext = pStream->codec;
			pStreamAudio = pStream;
			fAudioFile = fopen(pOutputAudioFile, "wb");
			if (!fAudioFile)
			{
				printf("con't open file!\n");
				goto end;
			}
			//������Ƶ֡  
			pFrame = av_frame_alloc();
			if (pFrame == NULL)
			{
				av_freep(&videoDstData[0]);
				printf("alloc audio frame error\n");
				goto end;
			}
			//���ݱ�����id���ұ�����  
			pAudioDec = avcodec_find_decoder(pStream->codec->codec_id);
			if (pAudioDec == NULL)
			{
				printf("���ұ�����ʧ��!\n");
				goto end;
			}
			if (avcodec_open2(pStream->codec, pAudioDec, nullptr) != 0)
			{
				printf("�򿪱�����ʧ��!\n");
				goto end;
			}
		}
	}
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;

	//��ȡý�����ݰ�  ����Ҫ���ڵ���0  
	while (av_read_frame(pFormatCtx, &pkt) >= 0)
	{
		AVPacket oriPkt = pkt;
		do
		{
			//����ÿ�������������  
			ret = decode_packet(&gotFrame, 0);
			if (ret < 0)
				break;
			//ָ�����  �����ڴ����  
			pkt.data += ret;
			pkt.size -= ret;
			//  
		} while (pkt.size > 0);
		//�ͷ�֮ǰ����Ŀռ�  ��ȡ��ϱ����ͷŰ�  
		av_free_packet(&oriPkt);
	}

end:
	//�ر���Ƶ������  
	avcodec_close(pVideoContext);
	//�ر���Ƶ������  
	avcodec_close(pAudioContext);
	avformat_close_input(&pFormatCtx);
	if (fVideoFile != NULL)
	{
		fclose(fVideoFile);
		fVideoFile = NULL;
	}
	
	if(fAudioFile != NULL)
	{
		fclose(fAudioFile);
		fAudioFile = NULL;
	}
	
	//�ͷű���֡  
	//avcodec_free_frame(&pFrame);
	av_free(&pFrame);
	//�ͷ���Ƶ������  
	av_free(videoDstData[0]);
	return 0;
}

int main(int argc, char*argv[])
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

#if 0
	//SDL_AudioSpec  
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = out_sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = out_channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = out_nb_samples;
	wanted_spec.callback = fill_audio;
	wanted_spec.userdata = pCodecCtx;

	if (SDL_OpenAudio(&wanted_spec, NULL)<0) {
		printf("can't open audio.\n");
		return -1;
	}
#endif
	//��������  
	pWindow = SDL_CreateWindow("YUV420P", 200, 100, 800, 600, 0);
	//����Ӳ������   
	pRender = SDL_CreateRenderer(pWindow, -1, 0);
	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = 1280;
	dstrect.h = 720;
	//����һ������  ���ÿ���Lock  YUV420P ��ʽ 1280*720   
	pTexture = SDL_CreateTexture(pRender, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, 1280, 720);
	Demuxing(argc, argv);
	//�ͷ�  
	SDL_RenderClear(pRender);
	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRender);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
	return  0;
}

#endif

#endif