#include <stdio.h>
#include "Stream.h"
#include <atlimage.h>

void saveImage(const char *filename, unsigned char *data, int width, int height) {
	CImage image;
	image.Create(width, height, 32, 0);
	//����д��ÿ�����ص��RGBֵ
	for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++) {
		image.SetPixelRGB(i, j,
			data[j * width * 3 + i * 3 + 0],
			data[j * width * 3 + i * 3 + 1],
			data[j * width * 3 + i * 3 + 2]);
	}
	image.Save(filename);
}


int decoder(AVFormatContext** fmt_ctx,AVCodecContext** dec_ctx, int video_stream_index,AVFrame *pFrame,AVFrame *pFrameColor, int* width, int* height)
{
	AVPacket packet;
	int i = 0;
	int frameFinished;
	uint8_t *buffer;
	int numBytes;
	char filename[32];
	
	//����ͼ������ռ��С������ռ�
	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, (*dec_ctx)->width, (*dec_ctx)->height);
	buffer = (uint8_t*)av_malloc(numBytes);
	avpicture_fill((AVPicture *)pFrameColor, buffer, AV_PIX_FMT_RGB24, (*dec_ctx)->width, (*dec_ctx)->height);

	//��ȡ��Ƶ֡������õ�ͼ��
	while (av_read_frame(*fmt_ctx, &packet) >= 0) {
		if (packet.stream_index == video_stream_index) {
			avcodec_decode_video2(*dec_ctx, pFrame, &frameFinished, &packet);

			//�õ��ѽ����ͼ��
			if (frameFinished)
			{
				if (pFrame->key_frame==1)			//�ж��Ƿ�Ϊ�ؼ�֡
				{
					struct SwsContext *img_convert_ctx = NULL;
					img_convert_ctx = sws_getCachedContext(img_convert_ctx, (*dec_ctx)->width,
						(*dec_ctx)->height, (*dec_ctx)->pix_fmt,
						(*dec_ctx)->width, (*dec_ctx)->height,
						AV_PIX_FMT_RGB24, SWS_BICUBIC,
						NULL, NULL, NULL);
					if (!img_convert_ctx) {
						fprintf(stderr, "Cannot initialize sws conversion context\n");
						exit(1);
					}

					//��������ͼ���ʽת��ΪRGB24
					sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data,
						pFrame->linesize, 0, (*dec_ctx)->height, pFrameColor->data,
						pFrameColor->linesize);
					
					//��ͼ��д��bmp�ļ�
					sprintf(filename, "result%d.bmp", ++i);
					saveImage(filename, pFrameColor->data[0], (*dec_ctx)->width, (*dec_ctx)->height);
					printf("keyframe %d get\n", i);
				}
			}
		}
		av_free_packet(&packet);
	}
	printf("finished");
	//�ͷ���Ϣ
	av_free(buffer);
	av_free(pFrameColor);
	av_free(pFrame);
	avcodec_close(*dec_ctx);
	avformat_close_input(fmt_ctx);
}

