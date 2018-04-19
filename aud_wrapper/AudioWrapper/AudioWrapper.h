/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AudioWrapper.h
 * Author 	  : zhaokun
 * Description: AudioWrapper
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef AUDIO_WRAPPER_H____
#define AUDIO_WRAPPER_H____

	#include "AudioCommon.h"
	#include "AudioBase.h"
	
	/**
	 * @brief wrapper class
	 * @function:
	 * 	1. provide an interface function to the outside
	 *  2. call inside function
	 */
	class AudioWrapper {

		public:

			AudioWrapper();
			~AudioWrapper();

			AudioBase * audioBase;

			/** 
			 * @brief open function to the outside
			 * @param handle
			 * @return 0:success -1:error
			 */
			int audin_wrapper_open(OpenStructT * pOpen);
			/** 
			 * @brief read function to the outside
			 * @param 
			 * @pcm:handle
			 * @return 0:success -1:error
			 */
			int audin_wrapper_read(ReadStructT * pRead);

			/** 
			 * @brief close function to the outside
			 * @param handle
			 * @return 0:success -1:error
			 */
			int audin_wrapper_close(CloseStructT * pClose);		
		
	};
#endif

