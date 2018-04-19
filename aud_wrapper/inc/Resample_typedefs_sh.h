 /***************************************************************************
  * 
  * Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
  * 
  **************************************************************************/
 
 /**
  * @file typedefs_sh.h
  * @author songhui(com@baidu.com)
  * @date 2015/08/28 17:30:26
  * @brief 
  *  
  **/

#ifndef  __Resample_TYPEDEFS_SH_H_
#define  __Resample_TYPEDEFS_SH_H_

typedef enum
{
	General_ArrayEquiSpacingLine,	//	Equi-spacing line
	General_ArraySymmetricLine,	//	Symmetric line
	General_ArrayLShape,           //  L shape
	General_ArrayArbitrary,		//	Arbitrary
} General_ArrayGeometric;

const float c = 340.0f;	//	sound speed

//typedef	struct
//{
//	float	rho;		//	distance
//	float	phi;		//	horizontal angle
//	float	theta;		//	vertical angle
//} PolarCoord;

typedef	struct
{
	float	x;		//	left to right
	float	y;		//	near to far
	float	z;		//	low to high
} PlaneCoord;

/*******************************************

				3	2	1	0
	mic	array	o	o	o	o


					
	 angle -  <----Speaker---->  angle +	
	 TDE -                       TDE +
									
*********************************************/

typedef struct {
    float  rho;        ///< distance, currently must be 1
    float  phi;        ///< horizontal angle, value range from -PI/2 to PI/2 (PI is 3.1415926535)
    float  theta;      ///< vertical angle, currently must be zero
} PolarCoord;

typedef struct
{
	int nIndexR;	//	the index of the right microphone (when speak faces to array)
	int nIndexL;	//	the index of the left microphone (should be larger than nIndexR)
} PairList;

#ifndef MAX_INPUT_CHANNEL_NUM
#define MAX_INPUT_CHANNEL_NUM   (16) // cui // hui
#endif

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int BOOL;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef unsigned long ULONG;
typedef LONG HRESULT;

#define _MAX_PATH   260 /*  max. length of full pathname */

#define S_OK ((HRESULT)0L)
#define S_FALSE ((HRESULT)1L)

#define FALSE false
#define TRUE true

const DWORD DEF_ASIO_SAMP_RATE = 48000UL;
const DWORD DEF_KERNEL_SAMP_RATE = 16000UL;
const DWORD DEF_KERNEL_BLOCK_SIZE = 128;

const DWORD DEF_DOWN_FACTOR = DEF_ASIO_SAMP_RATE / DEF_KERNEL_SAMP_RATE;
const DWORD DEF_ASIO_BLOCK_SIZE = DEF_KERNEL_BLOCK_SIZE * DEF_DOWN_FACTOR;

#ifndef	PI
#define	PI 3.1415926535f
#endif

#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef EPSILON
#define EPSILON 1e-5
#endif

#endif  //__TYPEDEFS_SH_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
