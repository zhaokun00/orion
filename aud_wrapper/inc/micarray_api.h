 /***************************************************************************
  * 
  * Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
  * 
  **************************************************************************/
 
 /**
  * @file MicArrayAPI.h
  * @author songhui(com@baidu.com)
  * @date 2015/09/09 11:26:10
  * @brief 
  *  
  **/

#ifndef  __MICARAAY_API_H_
#define  __MICARRAY_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

	enum MAPM_FLAG {
		LOCATION
	};

/*  @brief Set Param (Algorithm selection) 
 *  1  AEC only
 *  2 Array Processing only
 *  4  Post Processing only
 */
int mapm_setparam(void* st, unsigned int enable);

/*  @brief Get Param */
int mapm_getparam(void* st, int enablevalue);

/*@brief Set source location after wake up is successed*/
int mapm_set_flag(void *st, enum MAPM_FLAG flag_type);

/*@brief Compute wakeup location result by wakeupTimeInfo from wakeup SDK, and set directed VAD*/
int mapm_compute_wakLoca(void *st, struct objWakeupTimeInfo* wakeupTimeInfo, float *wakupLoca_buff);

/*@brief set OneshotHoldingLen 200ms?*/
int mapm_Set_OneshotHoldingLen(void *st, int HoldingLen);

/*@brief get OneshotMode, 0: normal(say hey),		1: Oneshot(not say hey)*/
int mapm_get_OneshotMode(void *st, unsigned int *OneshotMode_buff);

/*@brief Get GSC out data*/
int mapm_get_GSCdata(void *st, short  *pGSCdata);

/*@brief Get LocaInfo for debug*/
int mapm_get_LocaInfo(void *st, float  *LocaInfo);

/*@brief Get Data*/
//int mapm_getdata(void *st,enum MAPM_DATA data_type, short *data_buff, int datalen);

/* @brief Init MAPM Instance */
void* mapm_init(int nFs, int sensor_num, float d, float r, int ch_num);

/* @brief MAPM Process */
int mapm_process(void* st, char* inputdata, short *wakeup_data, short *recog_data, int inputdata_length);

/* @brief Push reference data into the lib, 128 samples one time */
int mapm_pushref(void* st, char* refdata, int refdata_length);

/*  @brief Reset mapm */
int mapm_reset(void* st);

/* @brief Exit MAPM Instance */
void mapm_exit(void* st);

#ifdef __cplusplus
}
#endif
#endif  //__MICARRAY_API_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
