/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : DataSource.h
 * Author 	  : zhaokun
 * Description: DataSource
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef DATA_SOURCE_H____
#define DATA_SOURCE_H____

	#include "AudioCommon.h"

	#define SAMPLE_FEQ_48K 48000
	#define SAMPLE_FEQ_16K 16000
	#define SALMPLE_FEQ_ALGO SAMPLE_FEQ_16K
	#define SAMPLE_TIME 8
	#define BYTES sizeof(short)

	#define PCM_ERROR_MAX 128
	
	struct pcm {
	    int fd;
	    unsigned int flags;
	    int running:1;
	    int prepared:1;
	    int underruns;
	    unsigned int buffer_size;
	    unsigned int boundary;
	    char error[PCM_ERROR_MAX];
	    struct pcm_config config;
	    struct snd_pcm_mmap_status *mmap_status;
	    struct snd_pcm_mmap_control *mmap_control;
	    struct snd_pcm_sync_ptr *sync_ptr;
	    void *mmap_buffer;
	    unsigned int noirq_frames_per_msec;
	    int wait_for_avail_min;
	};
	
	/**
	 * @brief source data base class
	 * @function:
	 * 	1. store processed microphone data
	 *  2. store processed speaker data 
	 * 
	 */
	class DataSource {

		public:
			
			MediaRingBufferT ringBufMic;
			MediaRingBufferT ringBufRes;

			DataSource();
//			DataSource(OpenStructT * data);
			virtual ~DataSource();
			/** 
			 * @brief initialize ring buffer
			 * @param 
			 * @return 0-sucess -1:error
			 */
			int init();
			virtual int initDataSource() = 0;
			/** 
			 * @brief
			 * @param 
			 * @return
			 */
			int close();
			virtual int closeDataSource() = 0; 
			/** 
			 * @brief read data,the subclass must be implemented
			 * @param 
			 * @return
			 */
			virtual int read(char *pMic,unsigned int micDataCount,char *pRes,unsigned int resDataCount) = 0;
			/** 
			 * @brief read device data
			 * @param 
			 * @return 
			 */
			int start();
			/** 
			 * @brief get microphone data
			 * @param pdata:buffer count:data's size
			 * @return 0-success -1:failed
			 */
			int getMicData(char *pData,unsigned int count);
			/** 
			 * @brief get res data
			 * @param pdata:buffer count:data's size
			 * @return 0-success -1:failed
			 */
			int getResData(char *pData,unsigned int count);
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
			
			void setMicSampleFre(unsigned int fre);
			void setResSampleFre(unsigned int fre);

			void setMicPerBufSize(unsigned int size);
			void setResPerBufSize(unsigned int size);
			void setMicBufNumber(unsigned int number);
			void setResBufNumber(unsigned int number);

			friend bool readHandlerEvent(void *ptr);
			
		protected:
			OpenStructT * pOpen;
		private:
			short * pMicDataBuf;
			short * pResDataBuf;
			short * pMicTmpDataBuf;
			short * pResTmpDataBuf;

			unsigned int micSampleFre;
			unsigned int resSampleFre;
			unsigned int micResampleFre;
			unsigned int resResampleFre;

			unsigned int micNumber;
			unsigned int resNumber;

			unsigned int micDataBufLen;
			unsigned int resDataBufLen;
			unsigned int micTmpDataBufLen;
			unsigned int resTmpDataBufLen;

			unsigned int micPerBufSize;
			unsigned int resPerBufSize;
			
			unsigned int micBufNumber;
			unsigned int resBufNumber;

			ThreadPosix  * capThread;
			bool capReady;
			CriticalSectionPosix * capReadyCx;
			
			void *pResampleMic;
    		void *pResampleRes;

			void setCapReady(bool val);
			bool isCapReady();

			void setMicResampleFre(unsigned int fre);
			void setResResampleFre(unsigned int fre);			
			
	};

#endif

