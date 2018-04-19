/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AudioBase.h
 * Author 	  : zhaokun
 * Description: AudioBase
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef AUDIO_BASE_H____
#define AUDIO_BASE_H____

	#include "AudioCommon.h"
	#include "AudioAlgo.h"
	#include "DataSource.h"	
	#include "AudioIpc.h"
	
	/**
	 * @brief Audio base class
	 */
	class AudioBase {

		public:
			/** 
			 * @brief initializes the member variables in the constructor
			 * @param
			 * @return
			 */
			AudioBase();
			virtual ~AudioBase();
			int open(OpenStructT * pOpen);
			/** 
			 * @brief open device,the subclass must be implemented
			 * @param
			 * @return
			 */
			virtual int openDevice(OpenStructT * pOpen) = 0;
			virtual int close(CloseStructT * pClose);
			/** 
			 * @brief close device,the subclass must be implemented
			 * @param
			 * @return
			 */
			virtual int closeDevice(CloseStructT * pClose) = 0;
			virtual int read(ReadStructT * pRead);			
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
			
		private:
			DataSource * dataSourceObj;
			AudioAlgo * audioAlgoObj;
			AudioIpc * audioIpcObj;
	};

#endif

