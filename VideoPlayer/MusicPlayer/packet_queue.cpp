#define __STDC_CONSTANT_MACROS      //ffmpegҪ��

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL.h>

#ifdef __cplusplus
}
#endif

#include "packet_queue.h"
/*======================================================================\
* Author     (����): i.sshe
* Date       (����): 2016/10/03
* Others     (����): ��ʼ������
\*=======================================================================*/
void packet_queue_init(PacketQueue *queue)
{
	//     memset(queue, 0, sizeof(PacketQueue));
	queue->first_pkt = NULL;
	queue->last_pkt = NULL;
	queue->mutex = SDL_CreateMutex();
	queue->cond = SDL_CreateCond();
}



/*======================================================================\
* Author     (����): i.sshe
* Date       (����): 2016/10/03
* Others     (����): ���
\*=======================================================================*/
int packet_queue_put(PacketQueue *queue, AVPacket *packet)
{
	AVPacketList   *pkt_list;

	// ???
	if (av_dup_packet(packet) < 0)
	{
		return -1;
	}

	pkt_list = (AVPacketList *)av_malloc(sizeof(AVPacketList));
	if (pkt_list == NULL)
	{
		return -1;
	}

	pkt_list->pkt = *packet;
	pkt_list->next = NULL;

	//����
	SDL_LockMutex(queue->mutex);

	if (queue->last_pkt == NULL)    //�ն�
	{
		queue->first_pkt = pkt_list;
	}
	else
	{
		queue->last_pkt->next = pkt_list;
	}

	queue->last_pkt = pkt_list;  //����queue->last_pkt = queue->last_pkt->next ����˼�����ǣ������˸���������
	queue->nb_packets++;
	queue->size += packet->size;
	SDL_CondSignal(queue->cond);      //???

	SDL_UnlockMutex(queue->mutex);

	return 0;
}

/*======================================================================\
* Author     (����): i.sshe
* Date       (����): 2016/10/03
* Others     (����): ����
\*=======================================================================*/
extern int quit;
int packet_queue_get(PacketQueue *queue, AVPacket *pkt, int block)
{
	AVPacketList   *pkt_list = NULL;
	int            ret = 0;

	SDL_LockMutex(queue->mutex);

	while (1)
	{
		if (quit)
		{
			ret = -1;
			break;
		}

		pkt_list = queue->first_pkt;
		if (pkt_list != NULL)         //�Ӳ��գ���������
		{
			queue->first_pkt = queue->first_pkt->next;    //pkt_list->next
			if (queue->first_pkt == NULL)
			{
				queue->last_pkt = NULL;
			}

			queue->nb_packets--;
			queue->size -= pkt_list->pkt.size;
			*pkt = pkt_list->pkt;          // ���Ƹ�packet��
			av_free(pkt_list);
			ret = 1;
			break;
		}
		else if (block == 0)
		{
			ret = 0;
			break;
		}
		else
		{
			SDL_CondWait(queue->cond, queue->mutex);
		}
	}

	SDL_UnlockMutex(queue->mutex);
	return ret;
}