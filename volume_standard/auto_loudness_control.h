#ifndef AUTO_LOUNDENSS_CONTROL_H____
	#define AUTO_LOUNDENSS_CONTROL_H____

	#include <math.h>
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>

	#define DEBUG_LOG

	#ifdef DEBUG_LOG

		#ifdef ANDROID
			#include <cutils/log.h>
			#define log(x...) ALOGD(x)
		#else
			#define log(x...) printf(x)
		#endif
	
	#else
		#define log(x...)
	#endif

	#ifndef min
		#define min(a,b)	((a) < (b)? (a) : (b))
	#endif
	
	#ifndef max
		#define max(a,b)	((a) > (b)? (a) : (b))
	#endif

	typedef struct WAV_HEADER_S  
	{  
		char			riffType[4];	//4byte,资源交换文件标志:RIFF	  
		unsigned int	riffSize;		//4byte,从下个地址到文件结尾的总字节数	
		char			waveType[4];	//4byte,wave文件标志:WAVE	  
		char			formatType[4];	//4byte,波形文件标志:FMT	  
		unsigned int	formatSize; 	//4byte,音频属性(compressionCode,numChannels,sampleRate,bytesPerSecond,blockAlign,bitsPerSample)所占字节数  
		unsigned short	compressionCode;//2byte,编码格式(1-线性pcm-WAVE_FORMAT_PCM,WAVEFORMAT_ADPCM)	
		unsigned short	numChannels;	//2byte,通道数  
		unsigned int	sampleRate; 	//4byte,采样率  
		unsigned int	bytesPerSecond; //4byte,传输速率  
		unsigned short	blockAlign; 	//2byte,数据块的对齐,计算公式:声道数 * 每个采所占的bit / 8
		unsigned short	bitsPerSample;	//2byte,采样精度  
		char			dataType[4];	//4byte,数据标志:data  
		unsigned int	dataSize;		//4byte,从下个地址到文件结尾的总字节数，即除了wav header以外的pcm data length  
	}WAV_HEADER; 
	

	#define TARGET_LEVEL			3000
	#define GAIN_MAX				20
	
	#define OFFSET_SAMPLE 			10
	#define READ_SAMPLE				8

	#define OFFSET_HEAD				44
	/** 
	* @brief get audio's gain
	* @param file path
	* @return gain
	*/	
	extern float get_gain(char *path);

	/** 
	* @brief calculate audio's level
	* @param file:file pointer，head:file head
	* @return audio's level
	*/
	int alc_audio_level(FILE *file,WAV_HEADER *head);

	/** 
	* @brief calculate per audio's level
	* @param audio:audio buffer,channels:channels,sapmles:samples
	* @return per audio's level
	*/
	float alc_audio_per_level(short *audio, int channels,int samples);

	/** 
	* @brief calculate gain
	* @param level:audio's level
	* @return gain
	*/	
	float alc_gain(int level);
	
	/** 
	* @brief calculate audio's data
	* @param audio:audio's data;channels:channels;samples:samples,gain:gain
	* @return level
	*/	
	void alc_run(short *audio, int channels, int sapmle,float gain);

#endif
