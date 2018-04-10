#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define __STDC_CONSTANT_MACROS      //ffmpegҪ��

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>  
#include <libswscale/swscale.h>  
#include <SDL.h>

#ifdef __cplusplus
}
#endif

//
#include "wrap_base.h"
#include "packet_queue.h"

#define AVCODE_MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
#define SDL_AUDIO_BUFFER_SIZE   1024    //

#define FILE_NAME               "/home/isshe/Music/WavinFlag.aac"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100

AVFrame         wanted_frame;
PacketQueue     audio_queue;
int      quit = 0;

void audio_callback(void *userdata, Uint8 *stream, int len);
int audio_decode_frame(AVCodecContext *pcodec_ctx, uint8_t *audio_buf, int buf_size);
/*
void packet_queue_init(PacketQueue *queue);
int packet_queue_put(PacketQueue *queue, AVPacket *packet);
int packet_queue_get(PacketQueue *queue, AVPacket *packet, int block);
*/
int main(int argc, char *argv[])
{
	AVFormatContext     *pformat_ctx = NULL;
	int                 audio_stream = -1;
	AVCodecContext      *pcodec_ctx = NULL;
	AVCodecContext      *pcodec_ctx_cp = NULL;
	AVCodec             *pcodec = NULL;
	AVPacket            packet;        //!
	AVFrame             *pframe = NULL;
	char                filename[256] = FILE_NAME;

	//SDL
	SDL_AudioSpec       wanted_spec;
	SDL_AudioSpec       spec;

	//ffmpeg ��ʼ��
	av_register_all();

	//SDL��ʼ��
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		fprintf(ERR_STREAM, "Couldn't init SDL:%s\n", SDL_GetError());
		exit(-1);
	}

	get_file_name(filename, argc, argv);

	//���ļ�
	if (avformat_open_input(&pformat_ctx, filename, NULL, NULL) != 0)
	{
		fprintf(ERR_STREAM, "Couldn't open input file\n");
		exit(-1);
	}

	//����ļ�����Ϣ
	//�ɰ����av_find_stream_info()
	if (avformat_find_stream_info(pformat_ctx, NULL) < 0)
	{
		fprintf(ERR_STREAM, "Not Found Stream Info\n");
		exit(-1);
	}

	//��ʾ�ļ���Ϣ��ʮ�ֺ��õ�һ������
	av_dump_format(pformat_ctx, 0, filename, false);

	//������Ƶ��
	if (find_stream_index(pformat_ctx, NULL, &audio_stream) == -1)
	{
		fprintf(ERR_STREAM, "Couldn't find stream index\n");
		exit(-1);
	}
	printf("audio_stream = %d\n", audio_stream);

	//�ҵ���Ӧ�Ľ�����
	pcodec_ctx = pformat_ctx->streams[audio_stream]->codec;
	pcodec = avcodec_find_decoder(pcodec_ctx->codec_id);
	if (!pcodec)
	{
		fprintf(ERR_STREAM, "Couldn't find decoder\n");
		exit(-1);
	}

	/*
	pcodec_ctx_cp = avcodec_alloc_context3(pcodec);
	if (avcodec_copy_context(pcodec_ctx_cp, pcodec_ctx) != 0)
	{
	fprintf(ERR_STREAM, "Couldn't copy codec context\n");
	exit(-1);
	}
	*/

	//������Ƶ��Ϣ, ��������Ƶ�豸��
	wanted_spec.freq = pcodec_ctx->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = pcodec_ctx->channels;        //ͨ����
	wanted_spec.silence = 0;    //���þ���ֵ
	wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;        //��ȡ��һ֡�����?
	wanted_spec.callback = audio_callback;
	wanted_spec.userdata = pcodec_ctx;

	//wanted_spec:��Ҫ�򿪵�
	//spec: ʵ�ʴ򿪵�,���Բ��������������ֱ����NULL�������õ�spec����wanted_spec���档
	//����Ὺһ���̣߳�����callback��
	//SDL_OpenAudio->open_audio_device(���߳�)->SDL_RunAudio->fill(ָ��callback������
	//������SDL_OpenAudioDevice()����
	if (SDL_OpenAudio(&wanted_spec, &spec) < 0)
	{
		fprintf(ERR_STREAM, "Couldn't open Audio:%s\n", SDL_GetError());
		exit(-1);
	}

	//���ò�����������ʱ����, swr_alloc_set_opts��in���ֲ���
	wanted_frame.format = AV_SAMPLE_FMT_S16;
	wanted_frame.sample_rate = spec.freq;
	wanted_frame.channel_layout = av_get_default_channel_layout(spec.channels);
	wanted_frame.channels = spec.channels;

	//��ʼ��AVCondecContext���Լ�����һЩ��������
	avcodec_open2(pcodec_ctx, pcodec, NULL);

	//��ʼ������
	packet_queue_init(&audio_queue);

	//������SDL_PauseAudioDevice()����,Ŀǰ�õ����Ӧ���Ǿɵġ�
	//0�ǲ���ͣ����������ͣ
	//���û������ͷŲ�������
	SDL_PauseAudio(0);              //ΪʲôҪ�����

									//��һ֡����
	while (av_read_frame(pformat_ctx, &packet) >= 0)     //��һ��packet�����ݷ���packet.data
	{
		if (packet.stream_index == audio_stream)
		{
			packet_queue_put(&audio_queue, &packet);
		}
		else
		{
			av_free_packet(&packet);
		}
	}
	getchar();      //...
	return 0;
}


//ע��userdata��ǰ���AVCodecContext.
//len��ֵ��Ϊ2048,��ʾһ�η��Ͷ��١�
//audio_buf_size��һֱΪ�����������Ĵ�С��wanted_spec.samples.��һ��ÿ�ν�����ô�࣬�ļ���ͬ�����ֵ��ͬ)
//audio_buf_index�� ��Ƿ��͵������ˡ�
//��������Ĺ���ģʽ��:
//1. �������ݷŵ�audio_buf, ��С��audio_buf_size��(audio_buf_size = audio_size;������ã�
//2. ����һ��callbackֻ�ܷ���len���ֽ�,��ÿ��ȡ�صĽ������ݿ��ܱ�len��һ�η����ꡣ
//3. �������ʱ�򣬻�len == 0��������ѭ�����˳���������������callback��������һ�η��͡�
//4. �����ϴ�û���꣬��β�ȡ���ݣ����ϴε�ʣ��ģ�audio_buf_size��Ƿ��͵������ˡ�
//5. ע�⣬callbackÿ��һ��Ҫ���ҽ���len�����ݣ����򲻻��˳���
//���û��������������û���ˣ�����ȡ�������ˣ����˳���������һ�������Դ�ѭ����
//������������Ϊstatic����Ϊ�˱����ϴ����ݣ�Ҳ������ȫ�ֱ��������Ǹо��������á�
void audio_callback(void *userdata, Uint8 *stream, int len)
{
	AVCodecContext *pcodec_ctx = (AVCodecContext *)userdata;
	int len1 = 0;
	int audio_size = 0;

	//ע����static
	//ΪʲôҪ����ô��
	static uint8_t         audio_buf[(AVCODE_MAX_AUDIO_FRAME_SIZE * 3) / 2];
	static unsigned int    audio_buf_size = 0;
	static unsigned int    audio_buf_index = 0;

	//��ʼ��stream��ÿ�ζ�Ҫ��
	SDL_memset(stream, 0, len);

	while (len > 0)
	{
		if (audio_buf_index >= audio_buf_size)
		{
			//����ȫ�����ͣ���ȥ��ȡ
			//�Զ����һ������
			audio_size = audio_decode_frame(pcodec_ctx, audio_buf, sizeof(audio_buf));
			if (audio_size < 0)
			{
				//��������
				audio_buf_size = 1024;
				memset(audio_buf, 0, audio_buf_size);
			}
			else
			{
				audio_buf_size = audio_size;
			}
			audio_buf_index = 0;      //�ص���������ͷ
		}

		len1 = audio_buf_size - audio_buf_index;
		//          printf("len1 = %d\n", len1);
		if (len1 > len)       //len1����len�󣬵���һ��callbackֻ�ܷ�len��
		{
			len1 = len;
		}

		//�³����� SDL_MixAudioFormat()����
		//�����Ƶ�� ��һ������dst�� �ڶ�����src��audio_buf_sizeÿ�ζ��ڱ仯
		SDL_MixAudio(stream, (uint8_t*)audio_buf + audio_buf_index, len, SDL_MIX_MAXVOLUME);
		//
		//memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
		len -= len1;
		stream += len1;
		audio_buf_index += len1;
	}

}


//������Ƶ��˵��һ��packet���棬���ܺ��ж�֡(frame)���ݡ�

int audio_decode_frame(AVCodecContext *pcodec_ctx,
	uint8_t *audio_buf, int buf_size)
{
	AVPacket   packet;
	AVFrame    *frame;
	int        got_frame;
	int        pkt_size = 0;
	//     uint8_t    *pkt_data = NULL;
	int        decode_len;
	int        try_again = 0;
	long long  audio_buf_index = 0;
	long long  data_size = 0;
	SwrContext *swr_ctx = NULL;
	int        convert_len = 0;
	int        convert_all = 0;

	if (packet_queue_get(&audio_queue, &packet, 1) < 0)
	{
		fprintf(ERR_STREAM, "Get queue packet error\n");
		return -1;
	}

	//     pkt_data = packet.data;
	pkt_size = packet.size;
	//     fprintf(ERR_STREAM, "pkt_size = %d\n", pkt_size);

	frame = av_frame_alloc();
	while (pkt_size > 0)
	{
		//          memset(frame, 0, sizeof(AVFrame));
		//pcodec_ctx:��������Ϣ
		//frame:����������ݵ�frame
		//got_frame:�����0������frameȡ�ˣ�����ζ�����˴���
		//packet:���룬ȡ���ݽ��롣
		decode_len = avcodec_decode_audio4(pcodec_ctx, frame, &got_frame, &packet);
		if (decode_len < 0) //�������
		{
			//�ؽ�, �������һֱ<0�أ�
			fprintf(ERR_STREAM, "Couldn't decode frame\n");
			if (try_again == 0)
			{
				try_again++;
				continue;
			}
			try_again = 0;
		}


		if (got_frame)
		{
			/*
				//�ö�����Ƶ������ȡ������������С
				data_size = av_samples_get_buffer_size(NULL,
				pcodec_ctx->channels, frame->nb_samples,
				pcodec_ctx->sample_fmt, 1);
				assert(data_size <= buf_size);
				//memcpy(audio_buf + audio_buf_index, frame->data[0], data_size);
			*/
			//chnanels: ͨ������, ��������Ƶ
			//channel_layout: ͨ�����֡�
			//����Ƶͨ��������һ��ͨ�����ֿ��Ծ����������������.ͨ����������������
			//���ָ���ǵ�����(mono)������������stereo), ������֮��İɣ�
			//���Դ�뼰��https://xdsnet.gitbooks.io/other-doc-cn-ffmpeg/content/ffmpeg-doc-cn-07.html#%E9%80%9A%E9%81%93%E5%B8%83%E5%B1%80
			if (frame->channels > 0 && frame->channel_layout == 0)
			{
				//��ȡĬ�ϲ��֣�Ĭ��Ӧ����stereo�ɣ�
				frame->channel_layout = av_get_default_channel_layout(frame->channels);
			}
			else if (frame->channels == 0 && frame->channel_layout > 0)
			{
				frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);
			}


			if (swr_ctx != NULL)
			{
				swr_free(&swr_ctx);
				swr_ctx = NULL;
			}

			//����common parameters
			//2,3,4��output������4,5,6��input������
			swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout,	(AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate, 
										frame->channel_layout, (AVSampleFormat)frame->format, frame->sample_rate, 0, NULL);
			//��ʼ��
			if (swr_ctx == NULL || swr_init(swr_ctx) < 0)
			{
				fprintf(ERR_STREAM, "swr_init error\n");
				break;
			}
			//av_rescale_rnd(): ��ָ���ķ�ʽ��64bit������������(rnd:rounding),
			//ʹ��a*b/c֮��Ĳ������������
			//swr_get_delay(): ���� �ڶ���������֮һ�������ǣ�1/frame->sample_rate)
			//AVRouding��һ��enum��1����˼��round away from zero.
			/*
			int dst_nb_samples = av_rescale_rnd(
			swr_get_delay(swr_ctx, frame->sample_rate) + frame->nb_samples,
			wanted_frame.sample_rate, wanted_frame.format,
			AVRounding(1));
			*/

			//ת����Ƶ����frame�е���Ƶת����ŵ�audio_buf�С�
			//��2��3����Ϊoutput�� ��4��5Ϊinput��
			//����ʹ��#define AVCODE_MAX_AUDIO_FRAME_SIZE 192000 
			//��dst_nb_samples�滻��, ���Ĳ���Ƶ����192kHz.
			convert_len = swr_convert(swr_ctx,	&audio_buf + audio_buf_index, AVCODE_MAX_AUDIO_FRAME_SIZE,	(const uint8_t **)frame->data, frame->nb_samples);
			printf("decode len = %d, convert_len = %d\n", decode_len, convert_len);
			//�����˶��٣����뵽������
			//pkt_data += decode_len;
			pkt_size -= decode_len;
			//ת�������Ч���ݴ浽�������audio_buf_index���
			audio_buf_index += convert_len;//data_size;
			//��������ת�������Ч���ݵĳ���
			convert_all += convert_len;
		}
	}
	return wanted_frame.channels * convert_all * av_get_bytes_per_sample((AVSampleFormat)wanted_frame.format);
	//     return audio_buf_index;
}