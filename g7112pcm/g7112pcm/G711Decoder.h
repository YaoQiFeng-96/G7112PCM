#pragma once

#include "headers.h"
class CG711Decoder
{
public:
	CG711Decoder();
	virtual ~CG711Decoder();

	bool Init(int sample_rate, int channels, AVSampleFormat sample_fmt, AVCodecID codec_id);
	void AddData(uint8_t* pData, int iSize);
	bool GetData(uint8_t* &pData, int* iSize);

private:
	const AVCodec		*m_codec;
	AVCodecContext		*m_codecCtx;
	AVPacket			*m_packet;
	AVFrame				*m_frame;
};

