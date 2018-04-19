/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AudioCM.h
 * Author 	  : zhaokun
 * Description: AudioCMRK29
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef AUDIO_CMRK29_H____
#define AUDIO_CMRK29_H____

	#include "AudioCommon.h"
	#include "AudioBase.h"
	#include "DataSourceCMRK29.h"
	
	#define CMRK29_MIC_NUMBER 6
	#define CMRK29_RES_NUMBER 1
	#define CMRK29_APM_LARRAY_FACRORY 0.0F
	#define CMRK29_APM_CARRAY_FACTORY 0.035F

	#define CMRK29_MIC_BUF_NUMBER 60
	#define CMRK29_RES_BUF_NUMBER 60
	#define CMRK29_ALOG_BUF_NUMBER 100

	#define CMRK29_MIC_PER_BUF_SZIE CMRK29_CAP_SIZE_16K * CMRK29_MIC_NUMBER * sizeof(short)
	#define CMRK29_RES_PER_BUF_SIZE CMRK29_CAP_SIZE_16K * CMRK29_RES_NUMBER * sizeof(short)
	#define CMRK29_ALOG_PER_BUF_SIZE CMRK29_CAP_SIZE_16K * 2 * sizeof(short)

	#define CMRK29_SAMPLE_FRE 48000
	#define CMRK29_SAMPLE_FRE 48000
	#define CMRK29_RESAMPLE_FRE 16000
	#define CMRK29_RESAMPLE_FRE 16000
	
	class AudioCMRK29:public AudioBase {

		public:

			AudioCMRK29();
			~AudioCMRK29();
			int openDevice(OpenStructT * pOpen);
			int closeDevice(CloseStructT *pClose);
		private:

			std::string className;

	};

#endif

