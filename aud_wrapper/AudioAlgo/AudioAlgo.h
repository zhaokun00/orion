/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AudioAlgo.h
 * Author 	  : zhaokun
 * Description: AudioAlgo
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */

#ifndef AUDIO_ALGO_H____
#define AUDIO_ALGO_H____

	#include "AudioCommon.h"
	#include "AlgoDump.h"
	#include "DataSource.h"
	
	#define ALGO_CAP_SIZE_16K 16 * 8
	#define AUDDEV_ONE_FRAME_SIZE_1CH ALGO_CAP_SIZE_16K * 1 * sizeof(short)
	#define AUDDEV_ONE_FRAME_SIZE_2CH ALGO_CAP_SIZE_16K * 2 * sizeof(short)
	#define APM_RATE_16K 16000
	#define APM_PROCESS_AEC_ENABLE		(0x00000001)
	#define APM_PROCESS_BF_ENABLE		(0x00000002)
	#define APM_PROCESS_SAPM_ENABLE 	(0x00000004)
	#define APM_PROCESS_ALL_ENABLE		(APM_PROCESS_AEC_ENABLE | APM_PROCESS_BF_ENABLE | APM_PROCESS_SAPM_ENABLE)

	#define WakeupWordNumMax 16
	#define ENABLE_LOCATION 0
	struct objWakeupTimeInfo {
		int WakTimeTickL;
		int WakTimeTickH;
		int WakWordNum;
		int WakWordStartTime[WakeupWordNumMax];
		int WakWordEndTime[WakeupWordNumMax];
	};
	

	/**
	 * @brief Audio algo class
	 */
	class AudioAlgo {

		public:
			AudioAlgo();
			~AudioAlgo();

			/** 
			 * @brief Audio algo init
			 * @param 
			 * @return 
			 */
			int init();
			/** 
			 * @brief Audio algo start
			 * @param 
			 * @return 
			 */
			int start();
			/** 
			 * @brief Audio algo close
			 * @param 
			 * @return 
			 */
			int close();
			/** 
			 * @brief get the data after the algorithm processed
			 * @param 
			 * @return 
			 */
			int getAlgoData(char *pData,unsigned int count);
			void* getAlgoHandle();
			/** 
			 * @brief get the dump's object
			 * @param 
			 * @return 
			 */
			AlgoDump * getAlgoDump();
			/** 
			 * @brief register datasource
			 * @param datasource
			 * @return 0-success -1-error
			 */
			int registerDataSource(DataSource * source);
			/** 
			 * @brief set microphone's number
			 * @param microphone number
			 * @return 
			 */
			void setMicNumber(unsigned int number);
			/** 
			 * @brief set reference signal's number
			 * @param reference signal's number
			 * @return 
			 */
			void setResNumber(unsigned int number);
			/** 
			 * @brief set linear distance
			 * @param distance
			 * @return 
			 */
			void setApmLarrayFactor(float factory);

			/** 
			 * @brief set ring distance
			 * @param distance
			 * @return 
			 */
			void setApmCarrayFactory(float factory);
			/** 
			 * @brief set algo per buffer's size
			 * @param size
			 * @return 
			 */
			void setalgoPerBufSize(unsigned int size);
			/** 
			 * @brief set algo buffer number
			 * @param buffer's number
			 * @return 
			 */
			void setalgoBufNumber(unsigned int number);
			int mapm_compute(struct objWakeupTimeInfo* wakeupTimeInfo, float *wakupLoca_buff);
			friend bool algoHandlerEvent(void *ptr);
			
		private:

			DataSource * dataSource;
			AlgoDump * algoDump; 
			//The subclass must set the following four parameters in order to initialize algorithm
			unsigned int micNumber;
			unsigned int resNumber;
			float apmLarrayFactor;
			float apmCarrayFactory;

			void *apmHandle;
			
			//alog thread
			ThreadPosix  * algoThread;
			bool algoReady;
    		CriticalSectionPosix *algoReadyCx;

			short * micBuf;
			short * resBuf;
			short * wakeUpBuf;
			short * reconBuf;
			short * outBuf;

			MediaRingBufferT ringBufAlgo;
			unsigned int algoPerBufSize;
			unsigned int algoBufNumber;

			void setAlgoReady(bool val);
			bool isAlgoReady();
			int interleaveAudioData(short *pIn01, short *pIn02, short *pOut, int sampleCnt);
		
	};
	
#endif

