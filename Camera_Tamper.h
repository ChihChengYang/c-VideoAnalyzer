/*
 * Camera_Tamper.h
 *
 *  Created on: 2010/6/4
 *      Author: Jeff Yang pc524
 */

#ifndef CAMERA_TAMPER_H_
#define CAMERA_TAMPER_H_

#include <xmmintrin.h>  //define for SSE
#include <emmintrin.h>  //define for SSE2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <math.h>
#include "IVAI_Parameters.h"

//add,2010/10/25
#include "CPUID.h"
#ifndef WIN32
#define _MM_ALIGN16 __attribute__( (__aligned__(16) ))// for linux
#endif
/*
 2 store type choice
 EX:
	|0|1|2|3|4|5|6|7|8|9| => STORE_FRAMES 9 ( Size of store frames is STORE_FRAMES + 1)
	SHORT_TERM_FRAMES 3
  0:
	[0]|1|2|3|4|5|[6]|7|8|<9> => <9> new frame compare with short term amount 3 [6] and long term amount 9 [0].
								Next new frame in this term |0| if no CAMERA TAMPER DETECTED.
  1:
	|0|1|[2]|<3>- => short term amount 3 ( Size of store short term frames is STORE_FRAMES + 1)
	|
	| SHORT_TO_LONG_INTERVAL_FRAMES control frame rate from short term copy to long term
	|
	->	[0]1|2|3|4|5|6|7| => long term amount 6 ( Size of store long term frames is STORE_FRAMES + 1)

	<3> new frame compare with short term [2] and long term [0].
	Next new frame store in short term |0| if no CAMERA TAMPER DETECTED.
 */
#define  SHORT_LONG_TERM_TYPE_CHOICE 0

/* Compare a value pointed to by 'ptr' to the 'center' value and
   increment pointer. Comparison is made by masking the most
   significant bit of an integer (the sign) and shifting it to an
   appropriate position. */
#define Compab_Mask_Inc(ptr,shift) { value |= ((unsigned int)(*center - *ptr - 1) & 0x80000000) >> (31-shift); ptr++; }

#define predicate 1

#define BINS_SIZE  256
	typedef struct Store_Frames_Result
	{		int Result[BINS_SIZE];
	} Store_Frames_Result;

//----------------------------------

//#define STORE_FRAMES  240//60// 20//40//9//120//90 /*   total array size (long term amount + short term amount) */
//#define SHORT_TERM_FRAMES 160//40//8//15// 3//60//30 /* short term amount */

//#define CAMERA_TAMPER_THRESHOLD 16000

//#define	CAMERA_TAMPER_WARNING_COUNT  120//30//5 /* WARNING when past CAMERA_TAMPER_WARNING_COUNT frames over CAMERA_TAMPER_THRESHOLD*/
//#define	CAMERA_TAMPER_WARNING_RELEASE_COUNT 240//5 /* RELEASE WARNING when past CAMERA_TAMPER_WARNING_RELEASE_COUNT frames under CAMERA_TAMPER_THRESHOLD*/

#if   SHORT_LONG_TERM_TYPE_CHOICE
	/* ¨C¹j SHORT_TO_LONG_INTERVAL_FRAMES from SHORT TERMS copy to LONG TERMS*/
	#define	SHORT_TO_LONG_INTERVAL_FRAMES 0//6 /* If 0 would be almost the same with 'SHORT_LONG_TERM_TYPE_CHOICE 0' but slowly. */
#endif

	//---------------------2010/08/31
	/* AUTO and MANUAL:0,
	 * only MANUAL:1 (Should set HandleCT->WarningTurnOffManual = 1 one time,system's parameters would be re-inital),
	 * DO NOTHING: neither 0 nor 1 */
	//#define AUTO_MANUAL_WARNINGTURNOFF  0
	/* AUTO WARNING TURN OFF release count*/
	//#define	CAMERA_TAMPER_WARNINGTURNOFF_COUNT (CAMERA_TAMPER_WARNING_RELEASE_COUNT<<1)
	//---------------------

//#pragma pack (4)
#if !SHORT_LONG_TERM_TYPE_CHOICE

typedef struct HandleCT
{
	Store_Frames_Result  *StoreFramesResult; /* Store frames in past , Size is STORE_FRAMES + 1*/
	int nWidth; /* input image width*/
	int nHeight; /* input image height*/
	//----------------
	unsigned char NonFristStrat;
	int FrameCount;
	int LongTermFrameCount;
	int ShortTermFrameCount;
	int WarningCount;
	int ReleaseCount;
	//---------------------2010/08/31
	int WarningTurnOffCount;
	unsigned char WarningTurnOffManual;
	//---------------------
	//----------------
	//---Add,Jeff Yang,2010/10/13
	CT_Parameters Param;
	//----------------
} HandleCT;

#else

typedef struct HandleCT
{
	Store_Frames_Result  *ShortTermFramesResult;/* Store Short Term frames in past , Size is SHORT_TERM_FRAMES + 1*/
	Store_Frames_Result  *LongTermFramesResult;/* Store Long Term frames in past , Size is STORE_FRAMES - SHORT_TERM_FRAMES + 1*/
	int nWidth; /* input image width*/
	int nHeight; /* input image height*/
	//-------------------
	unsigned char NonFristStrat;
	int FrameCount;
	int LongTermFrameCount;
	int ShortTermFrameCount;
	int WarningCount;
	int ReleaseCount;
	int ShortToLongIntervalFramesCount;
	//------------------
	//---Add,Jeff Yang,2010/10/13
	CT_Parameters Param;
				
} HandleCT;

#endif
//#pragma pack ()

//----------------------------------
/* Create *StoreFramesResult */
//Modify, Jeff Yang, 2010/11/08
//int Camera_Tamper_Created( HandleCT **Handle );
int Camera_Tamper_Created( HandleCT **Handle, const int width, const int height, CT_Parameters *CTParameters );
/* Destroy *StoreFramesResult */
int Camera_Tamper_Destroy( HandleCT *Handle );
/* unsigned char* pSourceGrayImage input source image(Gray), Running would return 1 if  Camera Tamper happened or return 0 normally*/
int Camera_Tamper_Running( HandleCT *Handle , unsigned char* pSourceGrayImage );
/* If parameters or image's resolution are changed would call this function again to reset related parameters.*/
void Camera_Tamper_Resetting(HandleCT *Handle);
//---Add,Jeff Yang,2010/10/13
int Camera_Tamper_Parameters( HandleCT *Handle, CT_Parameters *CTParameters );

#endif /* CAMERA_TAMPER_H_ */
