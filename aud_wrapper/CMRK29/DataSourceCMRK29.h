/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : DataSourceCMRK29.h
 * Author 	  : zhaokun
 * Description: DataSourceCMRK29
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef DATA_SOURCE_CMRK29_H____
#define DATA_SOURCE_CMRK29_H____

	#include "AudioCommon.h"
	#include "DataSource.h"

	#define CMRK29_RATE_48K 48000
	#define CMRK29_RATE_16K 16000
	
	#define CMRK29_CARD 0
	#define CMRK29_DEVICE 0
	#define CMRK29_CHANNELS 8
	#define CMRK29_RATE CMRK29_RATE_48K
	#define CMRK29_PERIOD_SIZE 128
	#define CMRK29_PERIOD_COUNT 4
	#define CMRK29_FORMAT PCM_FORMAT_S16_LE
	#define CMRK29_START_THRESHOLD 0
	#define CMRK29_STOP_THRESHOLD 0
	#define CMRK29_SILENCE_THRESHOLD 0
	
	#define CMRK29_CAP_SIZE_16K 16 * 8 //16:points per millisecond 8:capture's time
	#define CMRK29_CAP_SIZE_48K 48 * 8

	#define CMRK29_CHANNELS 8
	
	class DataSourceCMRK29:public DataSource {

		public:

			DataSourceCMRK29(OpenStructT * data);
			~DataSourceCMRK29();

			int initDataSource();
			int read(char *pMic,unsigned int micDataCount,char *pRes,unsigned int resDataCount);
			int closeDataSource();
			
		private:
			unsigned int card;
	    	unsigned int device;
	    	struct pcm *pcm;
	    	struct pcm_config config;
			struct pcm bad_pcm;

			struct pcm *outPcm;
			short rawBuf[CMRK29_CAP_SIZE_48K * CMRK29_CHANNELS];
			int deinterleaveAudioData(short *pIn, short *pOutMic, short *pOutSpk, int sampleCnt);

	};
	
#endif

