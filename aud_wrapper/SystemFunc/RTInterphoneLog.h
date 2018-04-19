// Add the log information for RTInterphone
// Jianqiang Wei
//////////////////////////////////////////////////////////////////////

#ifndef _RTINTERPHONELOG_H_
#define _RTINTERPHONELOG_H_

#include <android/log.h>

#define LOGENABLE

#ifdef LOGENABLE

#if 0
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "Auddev_wrapper", __VA_ARGS__);
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Auddev_wrapper", __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Auddev_wrapper", __VA_ARGS__);
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "Auddev_wrapper", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Auddev_wrapper", __VA_ARGS__);
#else
#define LOGV(...) printf(__VA_ARGS__);
#define LOGD(...) printf(__VA_ARGS__);
#define LOGI(...) printf(__VA_ARGS__);
#define LOGW(...) printf(__VA_ARGS__);
#define LOGE(...) printf(__VA_ARGS__);
#endif

#else // LOGENABLE

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)

#endif // LOGENABLE

#endif // _RTINTERPHONELOG_H_
