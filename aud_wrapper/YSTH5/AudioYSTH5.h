/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AudioYSTH5.h
 * Author 	  : zhaokun
 * Description: AudioYSTH5
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef AUDIO_YSTH5_H____
#define AUDIO_YSTH5_H____

	#include "AudioCommon.h"
	#include "AudioBase.h"
	#include "DataSourceYSTH5.h"
	
	#define H5_MIC_NUMBER 4
	#define H5_RES_NUMBER 1
	#define H5_APM_LARRAY_FACRORY 0.0F
	#define H5_APM_CARRAY_FACTORY 0.035F

	#define H5_MIC_BUF_NUMBER 100
	#define H5_RES_BUF_NUMBER 100
	#define H5_ALOG_BUF_NUMBER 100

	#define H5_MIC_PER_BUF_SZIE H5_CAP_SIZE_16K * H5_MIC_NUMBER * sizeof(short)
	#define H5_RES_PER_BUF_SIZE H5_CAP_SIZE_16K * H5_RES_NUMBER * sizeof(short)
	#define H5_ALOG_PER_BUF_SIZE H5_CAP_SIZE_16K * 2 * sizeof(short)

	#define H5_SAMPLE_FRE 48000
	#define H5_SAMPLE_FRE 48000
	#define H5_RESAMPLE_FRE 16000
	#define H5_RESAMPLE_FRE 16000
	
	class AudioYSTH5:public AudioBase {

		public:

			AudioYSTH5();
			~AudioYSTH5();
			int openDevice(OpenStructT * pOpen);
			int closeDevice(CloseStructT *pClose);
		private:

			std::string className;

	};

#endif

