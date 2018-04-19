#ifndef TINYALSA_LOG_H____
#define TINYALSA_LOG_H____

#ifdef TINYALSA_LOG_ENABLE__
#include <cutils/log.h>

// #define TA_DBG(x...) printf(x)
#define TA_DBG(x...) ALOGD(x)
//#define ALOGD(x) printf(x)
extern unsigned int g_recFrameNum;
#else
#define TA_DBG(x...) do { } while (0)
#endif

//#define ALOGD(x...) printf(x)

#define FUNC_E	TA_DBG(" +++ [%s] \n", __FUNCTION__)
#define FUNC_L	TA_DBG(" --- [%s] \n", __FUNCTION__)

#endif

