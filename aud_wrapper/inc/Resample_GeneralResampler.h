/*
********************************************************************************
**
**  Module Name:
**      GeneralResampler.h
**
**  Abstract:
**      Define and implement re-sampling class
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

#ifndef	__Resample_GENERAL_RESAMPLER_H__
#define	__Resample_GENERAL_RESAMPLER_H__
#include <stdio.h>
#include "Resample_FIRFiltering.h"


#define		FreeVector(x)		{if ( x != NULL ) {	delete [] x; x = NULL; }}

class CGeneralResampler
{
public:
	CGeneralResampler()
	{
		m_ppFloatOut=NULL;
		m_pShortOut=NULL;
		m_pFloatIn=NULL;
		m_FIRMRStates = NULL;
		m_pChannelSel = NULL;
	}
	bool Init(DWORD dwChannels,		//	number of channels
			  BOOL  *pChanelSel,    //  active channel list
			  DWORD	dwUpFactor,		//	upsampling factor
			  DWORD	dwDownFactor,	//	downsampling factor
			  DWORD	dwBaseSize,		//	input block size / dwDownFactor
			  DWORD dwFIRTaps,		//	number of FIR filter taps
			  DWORD	dwFIRRate,		//	sampling rate of FIR filter
			  float fcl,			//	lower cut-off frequency of bandpass filter
			  float fch			//	higher cut-off frequency of bandpass filter, 
			  );					//	recommended fch = 0.45*dwFIRRate / max(dwUpFactor,dwDownFactor)

	bool UnInit();		//	free memory

	ULONG Release()
	{
		UnInit();
		delete this;
		return 0;
	}

	/////////////////////////////////////////////////////////////
	//	prototype 1: short *pInput, multi-channel, continuous memory
	//	prototype 2: float **ppInput, pointer of pointer, x[ch][i]			
	HRESULT ResampleOneBlock(short * pInput);
	HRESULT ResampleOneBlock(float ** ppInput);

	//	return: pointer of output buffer
	float **GetFloatOutput();
	short *GetInt16Output();

protected:
	// Filter states
	FIRMRState **m_FIRMRStates;
	// Number of channels
	int m_nChannels;
	// Base Buffersize
	int m_nBaseSize;

	int m_nInputBlockSize;
	int m_nOutputBlockSize;

	float ** m_ppFloatOut;
	short * m_pShortOut;

	float * m_pFloatIn;

	BOOL * m_pChannelSel;
};

#endif /* __GENERAL_RESAMPLER_H__ */
