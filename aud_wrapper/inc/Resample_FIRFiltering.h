/*
********************************************************************************
**
**  Module Name:
**      FIRFiltering.h
**
**  Abstract:
**      Define and implement FIR filtering structs and APIs 
**
**  Authors:
**      Jianqiang Wei
**
**  Environment:
**      Complier requirements
**          Visual C++ 2010
**
**      Operating System requirements
**          Windows 7
**	
********************************************************************************
*/

#ifndef __Resample_FIR_FILTERING_H__
#define __Resample_FIR_FILTERING_H__

#include "Resample_typedefs_sh.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FIRCircularBuf // FIR filtering using the circular buffer in order to decrease
	                   // movement operations

/* Single-rate FIR filtering sturct and APIs */
typedef struct 
{
	float *FIRTaps;
	float *FIRBuf;
	DWORD  dwFIRTaps;
#ifdef FIRCircularBuf
	DWORD  dwBufInd;
#endif
} FIRState;

void FIRInitAlloc(FIRState **pFIRState, float *FIRTaps, DWORD dwFIRTaps, float *pDlyLine);
void FIRFree(FIRState *pFIRState);
void FIRFiltering(FIRState *pFIRState, float *pIn, float *pOut, DWORD dwBaseSize);
void FIRGetDlyLine(FIRState *pFIRState, float *pDlyLine);

/* Direct FIR filtering, not using the FIRState struct */
void FIRFiltering_Direct(float *pIn, float *pOut, int dwBaseSize, float *FIRTaps, int dwFIRTaps, float *pDlyLine, int *DlyLineIndex);

/* Multi-rate FIR filtering struct and APIs */
typedef struct 
{
	float *FIRTaps;
	float *FIRBuf;
	DWORD  dwFIRTaps;
	DWORD  dwUpFactor;
	DWORD  dwDownFactor;
#ifdef FIRCircularBuf
	DWORD  dwBufInd;
#endif
} FIRMRState;

void FIRMRInitAlloc(FIRMRState **pFIRMRState, float *FIRTaps, DWORD dwFIRTaps, DWORD dwUpFactor, DWORD dwDownFactor);
void FIRMRFree(FIRMRState *pFIRMRState);
void FIRMRFiltering(FIRMRState *pFIRMRState, float *pIn, float *pOut, DWORD dwBaseSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FIR_FILTERING_H__ */
