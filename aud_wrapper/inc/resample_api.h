#ifndef  __RESAMPLE_API_H_
#define  __RESAMPLE_API_H_
#include "Resample_GeneralResampler.h"

#ifdef __cplusplus
extern "C"
{
#endif


/* @brief Init Resample Instance */
void* resample_init(int ch_num, int InFreq, int OutFreq, int input_data_len_per_ch);

/* @brief Resample Process */
int resample_process(void* st, short* inputdata, short*outputdata, int total_inputdata_length);
//total_inputdata_length = ch_num * input_data_len_per_ch

/*  @brief Reset Resample */
int resample_reset(void* st);

/* @brief Exit Resample Instance */
void resample_exit(void* st);

#ifdef __cplusplus
}
#endif
#endif  //__RESAMPLE_API_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
