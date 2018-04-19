LOCAL_PATH:= $(call my-dir)
# - prebuild for license check library
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libcm_easr_pki_android
LOCAL_SRC_FILES := ./lib/libcm_easr_pki_android.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_PRELINK_MODULE := false
include $(BUILD_PREBUILT)
##############################################################################

###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libArrayProcessing_v1
LOCAL_SRC_FILES := ./lib/libArrayProcessing_v1.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
###############################################################################

###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libaudResampleprocess_v1 
LOCAL_SRC_FILES := ./lib/libaudResampleprocess_v1.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
###############################################################################

###############################################################################
$(shell cp -fr $(LOCAL_PATH)/lib/libNE10.so $(TARGET_OUT)/lib/)
$(shell cp -fr $(LOCAL_PATH)/lib/libNE10.so.10 $(TARGET_OUT)/lib/)
###############################################################################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
					external/tinyalsa/include \
					$(LOCAL_PATH)\
					$(LOCAL_PATH)/AudioWrapper/ \
					$(LOCAL_PATH)/inc \
					$(LOCAL_PATH)/SystemFunc \
					$(LOCAL_PATH)/AudioBase \
					$(LOCAL_PATH)/AudioAlgo \
					$(LOCAL_PATH)/DataSource \
					$(LOCAL_PATH)/AudioIpc

#in order to use map					
LOCAL_C_INCLUDES += \
					external/stlport/stlport \
					bionic


#add YST H5's platform include file
LOCAL_C_INCLUDES += $(LOCAL_PATH)/YSTH5

#add CMRK3229's platform include file
LOCAL_C_INCLUDES += $(LOCAL_PATH)/CMRK29
					
LOCAL_SRC_FILES := \
					./AudioWrapper/AudioWrapper.cpp \
					./AudioWrapper/auddev_priv_wrapper_api.cpp \
					./SystemFunc/utils_ringbuf.cpp \
					./SystemFunc/CriticalSectionPosix.cpp \
					./SystemFunc/EventPosix.cpp \
					./SystemFunc/ThreadPosix.cpp \
					./SystemFunc/eepipc_cn.cpp \
					./AudioBase/AudioBase.cpp \
					./AudioAlgo/AudioAlgo.cpp \
					./AudioAlgo/AlgoDump.cpp \
					./DataSource/DataSource.cpp \
					./AudioIpc/AudioIpc.cpp
					

LOCAL_SRC_FILES += \
				   ./AudioBase/ObjectFactory.cpp
				  
#Android.mk define string
LOCAL_CFLAGS += -DPLATFORM="\"AudioYSTH5\""
#LOCAL_CFLAGS += -DPLATFORM="\"AudioCMRK29\""

#add YST H5's platform source file
LOCAL_SRC_FILES += \
					./YSTH5/AudioYSTH5.cpp \
					./YSTH5/DataSourceYSTH5.cpp

#add CMRK29's platform source file
#LOCAL_SRC_FILES += \
					./CMRK29/AudioCMRK29.cpp \
					./CMRK29/DataSourceCMRK29.cpp
					
#add test file
#LOCAL_SRC_FILES += \
#					./AudioTest/auddev_priv_wrapper_client.cpp
					
LOCAL_MODULE := libaud_wrapper
#LOCAL_MODULE := auddev_priv_wrapper_client
LOCAL_SHARED_LIBRARIES:= libcutils libutils libstlport libtinyalsa libArrayProcessing_v1 libaudResampleprocess_v1 libcm_easr_pki_android 

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

##################add test start#####################################################
#LOCAL_MODULE_PATH := $(LOCAL_PATH)/AudioTest
#include $(BUILD_EXECUTABLE)
##################add test end#####################################################

###############################################################################
# audio app for wrapper usage example

#include $(CLEAR_VARS)

#LOCAL_C_INCLUDES := \
#					hardware/aw/audio_frame/aud_wrapper/AudioWrapper/

#LOCAL_SRC_FILES := \
#					./AudioTest/auddev_priv_wrapper_client.cpp
			
#LOCAL_MODULE := auddev_priv_wrapper_client
#LOCAL_SHARED_LIBRARIES:= libcutils libutils libaud_wrapper

#LOCAL_MODULE_TAGS := optional
#LOCAL_PRELINK_MODULE := false

#LOCAL_MODULE_PATH := $(LOCAL_PATH)/AudioTest
#include $(BUILD_EXECUTABLE)

#$(shell cp -fr $(TARGET_OUT)/lib/libaud_wrapper.so $(LOCAL_PATH)/AudioTest/)

###############################################################################

