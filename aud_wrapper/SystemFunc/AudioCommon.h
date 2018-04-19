/*
 **************************************************************************************
 * Copyright @ 北京猎户星空科技有限公司   1992-2099 All rights reserved
 * Filename   : AudioCommon.h
 * Author 	  : zhaokun
 * Description: AudioCommon
 * Date		  ：2017-12-07
 * Others	  :
 **************************************************************************************
 */

#ifndef AUDIO_COMMON_H____
#define AUDIO_COMMON_H____

	#include <stdio.h>
	#include <stdlib.h>
	#include <fcntl.h>
	#include <stdarg.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
	#include <poll.h>
	#include <sys/ioctl.h>
	#include <sys/mman.h>
	#include <sys/time.h>
	#include <limits.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <linux/ioctl.h>
	#include <sound/asound.h>
	#include <tinyalsa/asoundlib.h>
	#include <map>
	#include <unistd.h>
	#include <string>
	#include "cutils/properties.h"
	#include <cutils/log.h>
	
	#include "EventPosix.h"
	#include "CriticalSectionPosix.h"
	#include "ThreadPosix.h"
	#include "utils_ringbuf.h"
	#include "aud_wrapper_log.h"
	#include "error_code.h"
	#include "micarray_api.h"
	#include "auddev_priv_wrapper_api.h"
	#include "resample_api.h"
	#include "eepipc_cn.h"
	#include "CMIdentify.h"
	#include "ObjectFactory.h"

	#define ERROR_AUDIO -1
	#define SUCCESS_AUDIO 0
	
	#define DEBUG_LOG

#ifdef DEBUG_LOG

	#ifdef ANDROID
		#define log(x...) ALOGD(x)
	#else
		#define log(x...) printf(x)
	#endif
	
#else
	#define log(x...)
#endif

#endif

