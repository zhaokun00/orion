#include "auto_loudness_control.h"

float get_gain(char *path)
{
	float ret = 1;
	FILE * file = NULL;
	WAV_HEADER fileHead;
	int size = 0;
	int level = 0;
	float gain = 1;
	
	do {
		file = fopen(path,"rb");

		if(NULL == file) {
			log("open file failed\n");
			break;
		}

		size = fread(&fileHead,sizeof(char), sizeof(struct WAV_HEADER_S), file);

		if(size < sizeof(struct WAV_HEADER_S)) {
			log("read file's head failed\n");
			break;
		}

		level = alc_audio_level(file,&fileHead);
		gain = alc_gain(level);
		
		fclose(file);
		file = NULL;
		
	} while(0);

	return gain;
	
}

int alc_audio_level(FILE *file,WAV_HEADER *head)
{
	int retSize = 0;
	unsigned short channels = 0; //通道数
	unsigned int dataSize = 0; //数据区长度
	unsigned short sample = 0; //采样精度
	unsigned short perSampleSize = 0; //每一个采样点占的字节数
	unsigned int sumSampleNum = 0; //总的采样点数
	unsigned int offsetSampleNum = 0; //偏移的采样点数
	
	channels = head->numChannels; //通道数
//	dataSize = head->dataSize; //数据字节数

	fseek(file,0,SEEK_END);
	dataSize = ftell(file) - OFFSET_HEAD;	
	fseek(file,OFFSET_HEAD,SEEK_SET);
	
	sample = head->bitsPerSample; //每一个采样点占用的位数

//	log("channels = %d,dataSize = %d,sample = %d\n",channels,dataSize,sample);

	perSampleSize = channels * sample / 8;
	sumSampleNum = dataSize / perSampleSize; //总的采样点数

	offsetSampleNum = sumSampleNum / OFFSET_SAMPLE;

	fseek(file, offsetSampleNum * perSampleSize, SEEK_CUR); //跳过前面10%的字节数

	short *audioBufer = (short *)malloc(channels); //分配最小数据单元

	unsigned readSumSize = sumSampleNum / OFFSET_SAMPLE * READ_SAMPLE * perSampleSize; //读取数据的总字节数
	unsigned tempSumSize = 0;
	float sumLevel = 0;
	float level = 0;
	int averageLevel = 1;
	
	//计算中间80%采样点的数据s
	for(tempSumSize = 0;tempSumSize < readSumSize;) {
		retSize = fread(audioBufer,sizeof(char), sizeof(short) * channels, file);
		if(retSize < sizeof(short) * channels) {
			break;
		}
		tempSumSize += retSize;

		level = alc_audio_per_level(audioBufer,channels,1);
		sumLevel += level;
	}

	averageLevel = (int)sqrt(sumLevel / (channels * sumSampleNum / OFFSET_SAMPLE * READ_SAMPLE));

//	log("averageLevel = %d\n",averageLevel);

	free(audioBufer);
	audioBufer = NULL;
	
	return averageLevel;
}

float alc_audio_per_level(short *audio, int channels,int samples)
{
	int i;
	float level = 0;

	for (i = 0; i < channels * samples; i++) {
		level += audio[i] * audio[i];
	}

	return level;
}

float alc_gain(int level)
{
	float lev = (float)level;
	
	return min(GAIN_MAX, TARGET_LEVEL / lev);
}

void alc_run(short *audio, int channels, int sapmle,float gain)
{
	int i;

	for (i = 0; i < channels * sapmle; i++) {
		int out = (int)(gain * audio[i]);
		audio[i] = max(-32768, min(32767, out));
	}
}
