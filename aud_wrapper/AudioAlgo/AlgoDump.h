/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AlgoDump.h
 * Author 	  : zhaokun
 * Description: AudioAlgo
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */

#ifndef ALGO_DUMP_H____
#define ALGO_DUMP_H____

	#include "AudioCommon.h"

	#define MIC_BEFORE_DUMP_FILE_PATH "/data/local/test/dbg_algobefore_mic"
	#define MIC_AFTER_DUMP_FILE_PATH "/data/local/test/dbg_algoafter_mic"
	#define RES_DUMP_FILE_PATH "/data/local/test/spk"
	#define WAKEUP_DUMP_FILE_PATH "/data/local/test/dbg_wakeup.pcm"
	#define RECON_DUMP_FILE_PATH "/data/local/test/dbg_recon.pcm"
	#define DUMP_PROFILE_NAME_PATH "/sdcard/aw.so_profile/dump_flag"
	#define DUMP_BEFORE__PROFILE_NAME_PATH "/sdcard/aw.so_profile/dump_before_flag"

	class AlgoDump {

		public:

			static const int before = 0;
			static const int after = 1;
			AlgoDump();
			~AlgoDump();
			int open();
			int writeMicAndRes(char *pMic,unsigned int micDataSize,char *pRes,unsigned int resDataSize,int flag);
			int writeWakeAndRec(char *pWake,unsigned int wakeDataSize,char *pRec,unsigned int recDataSize);
			int close();

			void setMicFileNumber(unsigned int number);
			void setResFileNumebr(unsigned int number);

			friend void handlerSignal(int signal);

		private:
		
			FILE ** ppFile;
			
			unsigned int micFileNumber;
			unsigned int resFileNumber;
			static const unsigned int algoFileNumber = 2;
			unsigned int sumFileNumber;
			
			static bool enableBeforeDump;
			static bool enableDump;
			
			bool isDumpFile();
			bool isDumpBeforeFile();
			void saveLastRecFiles();
			void registerSignal();
			
	};
#endif

