#include "G711Decoder.h"

#pragma warning(disable:4996)

int main(int argc, char* argv[])
{
	CG711Decoder g711Decoder;
	if (!g711Decoder.Init(8000, 1, AV_SAMPLE_FMT_S16, AV_CODEC_ID_PCM_MULAW))
	{
		std::cout << "Decoder init failed." << std::endl;
		return -1;
	}
	FILE *in_file = fopen("8k_1_16.g711u", "rb");
	FILE *out_file = fopen("8k_1_16.pcm", "wb");

	uint8_t read_buf[1024] = { 0 };
	uint8_t* pData = nullptr;
	int size = 0;

	while (!feof(in_file))
	{
		int iRead = fread(read_buf, sizeof(uint8_t), 1024, in_file);
		if (iRead <= 0)
		{
			break;
		}
		g711Decoder.AddData(read_buf, iRead);

		while (g711Decoder.GetData(pData, &size))
		{
			fwrite(pData, 1, size, out_file);
			free(pData);
			pData = nullptr;
			size = 0;
		}
	}

	fclose(in_file);
	fclose(out_file);
	return 0;
}