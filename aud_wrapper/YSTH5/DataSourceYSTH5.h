/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : DataSourceYSTH5.h
 * Author 	  : zhaokun
 * Description: DataSourceYSTH5
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef DATA_SOURCE_YSTH5_H____
#define DATA_SOURCE_YSTH5_H____

	#include "AudioCommon.h"
	#include "DataSource.h"

	#define H5_RATE_48K 48000
	#define H5_RATE_16K 16000
	
	#define H5_CARD 3
	#define H5_DEVICE 0
	#define H5_CHANNELS 4
	#define H5_RATE H5_RATE_48K
	#define H5_PERIOD_SIZE 960
	#define H5_PERIOD_COUNT 4
	#define H5_FORMAT PCM_FORMAT_S16_LE
	#define H5_START_THRESHOLD 0
	#define H5_STOP_THRESHOLD 0
	#define H5_SILENCE_THRESHOLD 0
	
	#define H5_CAP_SIZE_16K 16 * 8 //16:points per millisecond 8:capture's time
	#define H5_CAP_SIZE_48K 48 * 8

	#define H5_MIC_NUM 4
	#define H5_RES_NUM 1

	#define H5_STOP_RECEIVE_PCM_DATA 0
	#define H5_START_RECEIVE_PCM_DATA 1
	#define H5_RECEIVE_PCM_DATA 2


	typedef int (*T_CPLAYBACK)(char* , unsigned int , unsigned int );
	
	class DataSourceYSTH5:public DataSource {

		public:

			DataSourceYSTH5(OpenStructT * data);
			~DataSourceYSTH5();

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
			short rawMicBuf[H5_CAP_SIZE_48K * H5_MIC_NUM];
			short rawResBuf[2*H5_CAP_SIZE_48K];

			T_CPLAYBACK playBack;
			
			int deinterleaveMicData(short *pIn, short *pOutMic, int sampleCnt);
			int deinterleaveResData(short *pIn, short *pOutSpk, int sampleCnt);

	};
	
#endif

