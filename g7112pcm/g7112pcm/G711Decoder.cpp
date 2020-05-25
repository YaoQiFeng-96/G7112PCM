#include "G711Decoder.h"

CG711Decoder::CG711Decoder() :
	m_codec(nullptr), m_codecCtx(nullptr),
	m_packet(nullptr), m_frame(nullptr)
{
}


CG711Decoder::~CG711Decoder()
{
	if (nullptr != m_packet)
	{
		av_packet_free(&m_packet);
		m_packet = nullptr;
	}
	if (nullptr != m_frame)
	{
		av_frame_free(&m_frame);
		m_frame = nullptr;
	}
	if (nullptr != m_codecCtx)
	{
		avcodec_close(m_codecCtx);
		avcodec_free_context(&m_codecCtx);
		m_codecCtx = nullptr;
	}
}

bool CG711Decoder::Init(int sample_rate, int channels, AVSampleFormat sample_fmt, AVCodecID codec_id)
{
	m_codec = avcodec_find_decoder(codec_id);
	if (!m_codec)
	{
		fprintf(stderr, "codec not found. \n");
		return false;
	}
	m_codecCtx = avcodec_alloc_context3(m_codec);
	if (!m_codecCtx)
	{
		fprintf(stderr, "could not allocate codec context. \n");
		return false;
	}
	m_codecCtx->sample_rate = sample_rate;
	m_codecCtx->sample_fmt = sample_fmt;
	m_codecCtx->channels = channels;

	if (avcodec_open2(m_codecCtx, m_codec, NULL) < 0)
	{
		fprintf(stderr, "could not open codec. \n");
		return false;
	}
	m_packet = av_packet_alloc();
	if (nullptr == m_packet)
		return false;

	m_frame = av_frame_alloc();
	if (nullptr == m_frame)
		return false;

	return true;
}

void CG711Decoder::AddData(uint8_t * pData, int iSize)
{
	m_packet->size = iSize;
	m_packet->data = (uint8_t *)av_malloc(m_packet->size);
	memcpy(m_packet->data, pData, m_packet->size);

	int ret = av_packet_from_data(m_packet, m_packet->data, m_packet->size);
	if (ret < 0)
	{
		fprintf(stderr, "av_packet_from_data() error. \n");
		av_free(m_packet->data);
		return;
	}
	ret = avcodec_send_packet(m_codecCtx, m_packet);
	av_packet_unref(m_packet);
	if (ret < 0)
	{
		fprintf(stderr, "avcodec_send_packet() error. \n");
	}
}

bool CG711Decoder::GetData(uint8_t *& pData, int * iSize)
{
	int ret = avcodec_receive_frame(m_codecCtx, m_frame);
	if (ret < 0)
	{
		//fprintf(stderr, "avcodec_receive_frame() error. \n");
		return false;
	}
	int data_size = av_get_bytes_per_sample(m_codecCtx->sample_fmt);
	if (data_size < 0)
	{
		fprintf(stderr, "failed to calculate data size. \n");
		return false;
	}
	int size = m_frame->nb_samples * m_codecCtx->channels * data_size;
	pData = (uint8_t *)malloc(size);
	memcpy(pData, m_frame->data[0], size);
	*iSize = size;
	return true;
}
