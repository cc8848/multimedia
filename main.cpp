#include <stdio.h>
#include "decoder.h"

int main()
{
	//��ʼ��������֡
	AVFormatContext *fmt_ctx = NULL;
	AVCodecContext *dec_ctx = NULL;	
	AVFrame *pf=av_frame_alloc();
	AVFrame *pfc = av_frame_alloc();
	int video_stream_index;
	int width, height;
	av_register_all();

	//��ȡ��Ƶ���⸴��
	video_stream_index=getStream(&fmt_ctx, &dec_ctx, "test.mp4");
	
	//���벢����ؼ�֡����ΪbmpͼƬ���
	decoder(&fmt_ctx, &dec_ctx, video_stream_index,pf, pfc, &width,&height);
	
}

