/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AudioIpc.h
 * Author 	  : zhaokun
 * Description: AudioDirection
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */
 
#ifndef AUDIO_IPC_H____
#define AUDIO_IPC_H____

	#include "AudioCommon.h"
	#include "AudioAlgo.h"

	enum IPC_MSG_TYPE {
		DIR_8MS_MSG_TYPE = 0x01,
		DIR_WAKEUP_MSG_TYPE = 0x02,
		DIR_ONESHOT_MODE_TYPE = 0x04
	};
	
	enum IPC_DATA_TYPE {
		DIR_DATA_TYPE = 0x01,
		TI_DATA_TYPE,
		LIC_DATA_TYPE
	};
	
	#define LICENSE_PRAMS_LEN_MAX	(128)
	struct LIC_PRAMS_INFO_S {
		unsigned int appID;
		char pktName[LICENSE_PRAMS_LEN_MAX];
		char appCode[LICENSE_PRAMS_LEN_MAX];
		char licPath[LICENSE_PRAMS_LEN_MAX];
		char pltform[LICENSE_PRAMS_LEN_MAX];
		char cUID[LICENSE_PRAMS_LEN_MAX];
	
	};
	
	typedef union Hal2AppMsgBodyS {
		void *dir_8ms;
		int dir_wakeup;
	} Hal2AppMsgBodyT;
	
	typedef struct Hal2AppMsgS {
		int type;
		Hal2AppMsgBodyT body;
	} Hal2AppMsgT;
	
	typedef union AppMsgBodyS {
		int dir_val;
		struct objWakeupTimeInfo wakloca_timeinfo;
		struct LIC_PRAMS_INFO_S license_info;
	} AppMsgBodyT;
	
	typedef struct AppDirMsgS {
		int type;
		AppMsgBodyT body;
	} AppDirMsgT;
	
	class AudioIpc {

		public:
			AudioIpc(AudioAlgo * obj);
			AudioIpc();

			int init();
			int start();
			int release();

			friend bool handlerEvent(void *ptr);
		private:

			AudioAlgo * algo;
		    int fdDir;
    		int fdDirCl;

			AppDirMsgT dirMsg;
    		Hal2AppMsgT halMsg;
			int securityVerify;
			int sverifyDone;
			ThreadPosix  * dirCtrlThread;
			bool dirCtrlReady;
			CriticalSectionPosix * dirCtrlReadyCx;

			void setDirCtlReady(bool val);
			bool isDirCtlReady();
			int strRangeValidation(const char *str, unsigned int maxLen);
	};

#endif
