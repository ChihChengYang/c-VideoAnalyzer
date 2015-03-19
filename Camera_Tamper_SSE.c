/*
 ============================================================================
 Name        :
				Camera_Tamper_SSE.c
 Author      :
				Jeff Yang
 Version     :
				v1.0 Created on: 2010/6/04
				v1.2 Created on: 2010/6/07 Add SHORT_LONG_TERM_TYPE_CHOICE
				v1.3 Created on: 2010/6/25 Move globe var. to Handle
				v1.4 Created on: 2010/7/02 Modify Handle -> *Handle
				v1.5 Created on: 2010/9/01 Add AUTO_MANUAL_WARNINGTURNOFF
				v1.6 Modify on : 2010/10/11 Change to SSE version for Linux (Camera_Tamper.c to Camera_Tamper_SSE.c)
				v1.7 Modify on : 2010/10/25 Check CPUID forSSE version  
 Copyright   :
				Jeff Yang
 Description :
				 Input frame rate 3fps...
 ============================================================================
 */

#include "Camera_Tamper.h"

#if   SHORT_LONG_TERM_TYPE_CHOICE
static int LONG_TERM_FRAMES;// = STORE_FRAMES - SHORT_TERM_FRAMES;
#endif

static int BinsSize = sizeof(int)*BINS_SIZE;
	
//---Add,Jeff Yang,2010/10/25
void LBP_Histogram_C( HandleCT *Handle, unsigned char*  img,  int*  result );
void LBP_Histogram_SSE( HandleCT *Handle, unsigned char*  img,  int*  result );
void (*LBP_Histogram)( HandleCT *Handle, unsigned char*  img,  int*  result );
//---

//----------------------------
int Camera_Tamper_Parameters( HandleCT *Handle, CT_Parameters *CTParameters )
{
	if(CTParameters->STORE_FRAMES<=0 || (CTParameters->SHORT_TERM_FRAMES<=0 || CTParameters->SHORT_TERM_FRAMES >= CTParameters->STORE_FRAMES) ||
		CTParameters->CAMERA_TAMPER_THRESHOLD<=0 || CTParameters->CAMERA_TAMPER_WARNING_COUNT<=0 || CTParameters->CAMERA_TAMPER_WARNING_RELEASE_COUNT<=0)
	{
		printf("ERROR------ %s %d -------\n",__FILE__, __LINE__ );
		return -1;
	}
	else
	{
		free(Handle->StoreFramesResult);
		Handle->StoreFramesResult = 0;

		Handle->Param.STORE_FRAMES = CTParameters->STORE_FRAMES;
		Handle->Param.SHORT_TERM_FRAMES = CTParameters->SHORT_TERM_FRAMES;
		Handle->Param.CAMERA_TAMPER_THRESHOLD = CTParameters->CAMERA_TAMPER_THRESHOLD;
		Handle->Param.CAMERA_TAMPER_WARNING_COUNT = CTParameters->CAMERA_TAMPER_WARNING_COUNT;
		Handle->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT = CTParameters->CAMERA_TAMPER_WARNING_RELEASE_COUNT;
		Handle->Param.AUTO_MANUAL_WARNINGTURNOFF = CTParameters->AUTO_MANUAL_WARNINGTURNOFF;
		Handle->Param.CAMERA_TAMPER_WARNINGTURNOFF_COUNT = (Handle->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT<<1);


#if !SHORT_LONG_TERM_TYPE_CHOICE

		Handle->StoreFramesResult = (Store_Frames_Result *) malloc(sizeof(Store_Frames_Result) * (Handle->Param.STORE_FRAMES+1) );
		if( Handle->StoreFramesResult  ==  0)
		{
			printf("ERROR------ %s %d -------\n",__FILE__,__LINE__ );
			return -1;
		}

#else
		LONG_TERM_FRAMES = Handle->Param.STORE_FRAMES - Handle->Param.SHORT_TERM_FRAMES;

		Handle->ShortTermFramesResult = (Store_Frames_Result *) malloc(sizeof(Store_Frames_Result) * (Handle->Param.SHORT_TERM_FRAMES+1) );
		if( Handle->ShortTermFramesResult  ==  0)
			return -1;

		Handle->LongTermFramesResult = (Store_Frames_Result *) malloc(sizeof(Store_Frames_Result) *  (LONG_TERM_FRAMES+1) );
		if( Handle->LongTermFramesResult  ==  0)
			return -1;

		Handle->ShortToLongIntervalFramesCount = 0;
#endif

		Handle->ShortTermFrameCount = Handle->LongTermFrameCount = Handle->FrameCount = 0;
		Handle->NonFristStrat = 0;
		Handle->ReleaseCount =
		Handle->WarningCount = 0;
		Handle->WarningTurnOffCount = 0;
		Handle->WarningTurnOffManual = 0;

	}

	return 0;
}
// ========================================================================
//	Camera_Tamper_Created
//
//		Input:
//			HandleCT *Handle
//		Output:
//			none
//		Return :
//			 0 create array successfully
//			 -1 create array fail
//		Description :
//			    Input HandleCT *Handle to create Store_Frames_Result * array,
//			    and set some parameters.
//	========================================================================
int Camera_Tamper_Created( HandleCT **Handle, const int width, const int height, CT_Parameters *CTParameters )
{
	*Handle = (HandleCT *)malloc(sizeof(HandleCT));
	if( *Handle  ==  0)
	{
		printf("ERROR------ %s %d -------\n",__FILE__,__LINE__ );
		return -1;
	}
	(*Handle)->nHeight = height;
	(*Handle)->nWidth = width;
	//-------------------------------------------------------------------------------------------------
	//Added, Jeff Yang,   2010/10/13
	if(CTParameters == NULL)
	{
	    (*Handle)->Param.STORE_FRAMES = 240;
	    (*Handle)->Param.SHORT_TERM_FRAMES = 160;
	    (*Handle)->Param.CAMERA_TAMPER_THRESHOLD = 16000;
 	    (*Handle)->Param.CAMERA_TAMPER_WARNING_COUNT = 120;
	    (*Handle)->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT = 240;
	    (*Handle)->Param.AUTO_MANUAL_WARNINGTURNOFF = 0;
	    (*Handle)->Param.CAMERA_TAMPER_WARNINGTURNOFF_COUNT = ((*Handle)->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT<<1);
	}
	else
	{
		(*Handle)->Param.STORE_FRAMES =  CTParameters->STORE_FRAMES;
		(*Handle)->Param.SHORT_TERM_FRAMES =  CTParameters->SHORT_TERM_FRAMES;
		(*Handle)->Param.CAMERA_TAMPER_THRESHOLD =  CTParameters->CAMERA_TAMPER_THRESHOLD;
		(*Handle)->Param.CAMERA_TAMPER_WARNING_COUNT = CTParameters->CAMERA_TAMPER_WARNING_COUNT;
		(*Handle)->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT = CTParameters->CAMERA_TAMPER_WARNING_RELEASE_COUNT;
		(*Handle)->Param.AUTO_MANUAL_WARNINGTURNOFF = CTParameters->AUTO_MANUAL_WARNINGTURNOFF;
		(*Handle)->Param.CAMERA_TAMPER_WARNINGTURNOFF_COUNT = ((*Handle)->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT<<1);

	}
   //-------------------------------------------------------------------------------------------------	
	#if !SHORT_LONG_TERM_TYPE_CHOICE 
		(*Handle)->StoreFramesResult = (Store_Frames_Result *) malloc(sizeof(Store_Frames_Result) * ((*Handle)->Param.STORE_FRAMES+1) );
			if( (*Handle)->StoreFramesResult  ==  0)
				return -1;
	#else		
		LONG_TERM_FRAMES = (*Handle)->Param.STORE_FRAMES - (*Handle)->Param.SHORT_TERM_FRAMES;
		(*Handle)->ShortTermFramesResult = (Store_Frames_Result *) malloc(sizeof(Store_Frames_Result) * ((*Handle)->Param.SHORT_TERM_FRAMES+1) );
			if( (*Handle)->ShortTermFramesResult  ==  0)
				return -1;
		(*Handle)->LongTermFramesResult = (Store_Frames_Result *) malloc(sizeof(Store_Frames_Result) *  (LONG_TERM_FRAMES+1) );
			if( (*Handle)->LongTermFramesResult  ==  0)
				return -1;
		(*Handle)->ShortToLongIntervalFramesCount = 0;
	#endif

		(*Handle)->ShortTermFrameCount = (*Handle)->LongTermFrameCount = (*Handle)->FrameCount = 0;
		(*Handle)->NonFristStrat = 0;
		(*Handle)->ReleaseCount =
		(*Handle)->WarningCount = 0;
		//---------------------2010/08/31
	 	(*Handle)->WarningTurnOffCount = 0;
		(*Handle)->WarningTurnOffManual = 0;
		 //---------------------		
	//=================================================================
	//add,Jeff Yang,2010/10/25
	LBP_Histogram = LBP_Histogram_C;
	if (using_SSE)
	{
		LBP_Histogram = LBP_Histogram_SSE;
	}
	//=================================================================
	
	return 0;
}

//========================================================================
//Camera_Tamper_Destroy
//
//Input:
//	HandleCT *Handle
//Output:
//	none
//Return :
//	 0 Destroy array successfully
//	 -1 Destroy array fail
//Description :
//    Input HandleCT *Handle to destroy Store_Frames_Result * array.
//======================================================================== 
int Camera_Tamper_Destroy( HandleCT *Handle )
{

#if !SHORT_LONG_TERM_TYPE_CHOICE
	free(Handle->StoreFramesResult);
	Handle->StoreFramesResult = 0;
	free(Handle);
	Handle = 0;
#else
	free(Handle->ShortTermFramesResult);
	Handle->ShortTermFramesResult = 0;
	free(Handle->LongTermFramesResult);
	Handle->LongTermFramesResult = 0;
	free(Handle);
	Handle = 0;
#endif

	return 0;
}
/* ========================================================================
	 LBP_Histogram

		Input:
			HandleCT *Handle: the number of rows and columns in the image
			unsigned char* img: the image data, an array of rows*columns integers arranged in a horizontal raster-scan order
			int* result: an array of 256 integers. Will hold the 256-bin LBP histogram.
		Output:
			none
		Return :
			none
		Description :
			Calculate the LBP histogram for an integer-valued image. This is an
			optimized version of the basic 8-bit LBP operator. Note that this
			assumes 4-byte integers. In some architectures, one must modify the
			code to reflect a different integer size.
======================================================================== */
void LBP_Histogram_C( HandleCT *Handle, unsigned char *img,  int *result )
{
 	/*
	int leap = columns*predicate;
	int pred2 = predicate << 1;
	*/
 	/* predicate = 1 */
	int leap = Handle->nWidth;
	int pred2 = 2;
	/*Set up a circularly indexed neighborhood using nine pointers.*/
	unsigned char
		*p0 = img,
		*p1 = p0 + predicate,
		*p2 = p1 + predicate,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - predicate,
		*p6 = p5 - predicate,
		*p7 = p6 - leap,
		*center = p7 + predicate;
	unsigned int value;
	int r=0,c,nWidth = Handle->nWidth-pred2,nHeight = Handle->nHeight-pred2;
	memset(result,0,BinsSize); /* Clear result histogram */
	//	for (r=0; r<nHeight; r++)
	while(r<nHeight)
	{
		for (c=0; c<nWidth; c++)
		{
			value = 0;
			/* Unrolled loop */
			Compab_Mask_Inc(p0,0);
			Compab_Mask_Inc(p1,1);
			Compab_Mask_Inc(p2,2);
			Compab_Mask_Inc(p3,3);
			Compab_Mask_Inc(p4,4);
			Compab_Mask_Inc(p5,5);
			Compab_Mask_Inc(p6,6);
			Compab_Mask_Inc(p7,7);
			center++;
			result[value]++; /* Increase histogram bin value */
		}
		p0 += pred2; p1 += pred2; p2 += pred2;
		p3 += pred2; p4 += pred2; p5 += pred2;
		p6 += pred2; p7 += pred2; center += pred2;
		++r;
	}


}
//========================================================================
//	 LBP_Histogram
//
//		Input:
//			HandleCT *Handle: the number of rows and columns in the image
//			unsigned char* img: the image data, an array of rows*columns integers arranged in a horizontal raster-scan order
//			int* result: an array of 256 integers. Will hold the 256-bin LBP histogram.
//		Output:
//			none
//		Return :
//			none
//		Description :
//			Calculate the LBP histogram for an integer-valued image. This is an
//			optimized version of the basic 8-bit LBP operator. Note that this
//			assumes 4-byte integers. In some architectures, one must modify the
//			code to reflect a different integer size.
//==========================
//||  p0  ||  p1  ||  p2  ||        
//||========================
//||  p7  ||  C   ||  p3  ||
//||========================
//||  p6  ||  p5  ||  p4  ||
//==========================
//	 ======================================================================== 
void LBP_Histogram_SSE( HandleCT *Handle, unsigned char*  img,  int*  result )
{
 		
	_MM_ALIGN16 unsigned short sigp[8];
	_MM_ALIGN16 short andarray[8]={0x00ff,0x00ff,0x00ff,0x00ff,0x00ff,0x00ff,0x00ff,0x00ff};	
	__m128i p0_128i,p1_128i,p2_128i,p3_128i,p4_128i,p5_128i,p6_128i,p7_128i,c_128i;
    __m128i r0_128i,r1_128i,rc_128i,zero128i,and128i;
	//char *cp0,*cp1,*cp2,*cp3,*cp4,*cp5,*cp6,*cp7;
	//Modify ,Jeff Yang, 2010/10/11
	int leap = Handle->nWidth<<1;
	int pred2 = 2,offset=0;
	unsigned int value=0;//,temp;
			
		
	/*Set up a circularly indexed neighborhood using nine pointers.*/
	/*unsigned char
			*p0 = img,
			*p1 = p0 + predicate,
			*p2 = p1 + predicate,
			*p3 = p2 + leap,
			*p4 = p3 + leap,
			*p5 = p4 - predicate,
			*p6 = p5 - predicate,
			*p7 = p6 - leap,
			*center = p7 + predicate;
		
*/
	int r=0,c,numidx,nWidth = (Handle->nWidth)/16,nHeight = Handle->nHeight-pred2;
	memset(result,0,BinsSize); /* Clear result histogram */

    zero128i = _mm_setzero_si128 ();
	and128i = _mm_load_si128(andarray);
          
	while(r<nHeight)
	{
							
	    offset = r*Handle->nWidth;//data offset
		
		for (c=0; c<nWidth; c++)
		{
							    
         //load data from img,each pixel of nine grid compare with the centeral pixel of nine grid
		 //每次載入16筆資料
			p0_128i = _mm_loadu_si128((__m128i *)(img+offset));//16筆p0
			p1_128i = _mm_loadu_si128((__m128i *)(img+offset+1));//16筆p1
			p2_128i = _mm_loadu_si128((__m128i *)(img+offset+2));//16筆p2
			p3_128i = _mm_loadu_si128((__m128i *)(img+Handle->nWidth+offset+2));//16筆p3
			//Modify ,Jeff Yang, 2010/10/11
			 //leap = Handle->nWidth<<1
			p4_128i = _mm_loadu_si128((__m128i *)(img+(leap)+offset+2));//16筆p4
			p5_128i = _mm_loadu_si128((__m128i *)(img+(leap)+offset+1));//16筆p5
			p6_128i = _mm_loadu_si128((__m128i *)(img+(leap)+offset));//16筆p6
			p7_128i = _mm_loadu_si128((__m128i *)(img+Handle->nWidth+offset));//16筆p7
			c_128i  = _mm_loadu_si128((__m128i *)(img+Handle->nWidth+offset+1));//16筆center

				 //與中心鄰近的每一個點都會與九宮格的中心點比較，如果小於則結果為0xff大於則為0

				 //但是由於SSE bit的cmplt只支援signed type的運算但我們的資料為unsigned type，一般而言signed type如果大於127會變為負，所以我們進行以下的修正,
				 //1.先比較原先要比較的兩個值
				 //2.判斷這兩個值是否其中一個值為負值，我們所需要修的情形只有正值與負值比較及負值與正值比較的情形。至於正值與正值，負值與負值並不會影響輸出結果
				 //3.進行xor的運算，這是因為如果在unsinged 中負值實際是比正值大的，所以當負值與正值比較所得的值為ff但實際應該為0
				 //  相反的正值實際是比負值小，所以當正值與負值比所得值為0時實際應該為ff，所以才透過xor運算來修正數值當

						         
			rc_128i = _mm_cmplt_epi8 (c_128i,zero128i);//判斷中心點是否為負值
			
			r0_128i=_mm_cmplt_epi8 (c_128i,p0_128i);//p0與c進行比較如果小於則得到0xff否則為0
			r1_128i=_mm_cmplt_epi8 (p0_128i,zero128i);//判斷p0是否為負值
			p0_128i = _mm_xor_si128(r0_128i,r1_128i);//使用xor修正原始比較的結果
			p0_128i = _mm_xor_si128(p0_128i,rc_128i);

			r0_128i=_mm_cmplt_epi8 (c_128i,p1_128i);
			r1_128i=_mm_cmplt_epi8 (p1_128i,zero128i);
			p1_128i = _mm_xor_si128(r0_128i,r1_128i);
			p1_128i = _mm_xor_si128(p1_128i,rc_128i);

			r0_128i=_mm_cmplt_epi8 (c_128i,p2_128i);
			r1_128i=_mm_cmplt_epi8 (p2_128i,zero128i);
			p2_128i = _mm_xor_si128(r0_128i,r1_128i);
			p2_128i = _mm_xor_si128(p2_128i,rc_128i);

			r0_128i=_mm_cmplt_epi8 (c_128i,p3_128i);
            r1_128i=_mm_cmplt_epi8 (p3_128i,zero128i);
			p3_128i = _mm_xor_si128(r0_128i,r1_128i);
			p3_128i = _mm_xor_si128(p3_128i,rc_128i);

			r0_128i=_mm_cmplt_epi8 (c_128i,p4_128i);
			r1_128i=_mm_cmplt_epi8 (p4_128i,zero128i);
			p4_128i = _mm_xor_si128(r0_128i,r1_128i);
			p4_128i = _mm_xor_si128(p4_128i,rc_128i);

			r0_128i=_mm_cmplt_epi8 (c_128i,p5_128i);
			r1_128i=_mm_cmplt_epi8 (p5_128i,zero128i);
			p5_128i = _mm_xor_si128(r0_128i,r1_128i);
			p5_128i = _mm_xor_si128(p5_128i,rc_128i);

			r0_128i=_mm_cmplt_epi8 (c_128i,p6_128i);
            r1_128i=_mm_cmplt_epi8 (p6_128i,zero128i);
			p6_128i = _mm_xor_si128(r0_128i,r1_128i);
			p6_128i = _mm_xor_si128(p6_128i,rc_128i);
 
            r0_128i=_mm_cmplt_epi8 (c_128i,p7_128i);
			r1_128i=_mm_cmplt_epi8 (p7_128i,zero128i);
			p7_128i = _mm_xor_si128(r0_128i,r1_128i);
			p7_128i = _mm_xor_si128(p7_128i,rc_128i);


            //將16個8 bit的資料取出signed bit並將其儲存
			sigp[0] = _mm_movemask_epi8(p0_128i);
			sigp[1] = _mm_movemask_epi8(p1_128i);
			sigp[2] = _mm_movemask_epi8(p2_128i);
			sigp[3] = _mm_movemask_epi8(p3_128i);
			sigp[4] = _mm_movemask_epi8(p4_128i);
			sigp[5] = _mm_movemask_epi8(p5_128i);
			sigp[6] = _mm_movemask_epi8(p6_128i);
			sigp[7] = _mm_movemask_epi8(p7_128i);
 /*                                
				r0_128i = _mm_load_si128(sigp);//將8筆16bit的資料變為__m128i的格式
				r1_128i = _mm_and_si128(r0_128i,and128i);//將8筆16bit中的每一筆lower 8 bit變為0

				rc_128i =  _mm_srai_epi16 (r0_128i,8);
				rc_128i=   _mm_and_si128(rc_128i,and128i);//將8筆16bit中的每一筆higher 8 bit變為0

				//__m128i _mm_packs_epi16 (__m128i a, __m128i b);
                //r0 := SignedSaturate(a0)
                //r1 := SignedSaturate(a1)
                //...
				//r7 := SignedSaturate(a7)
                //r8 := SignedSaturate(b0)
                //r9 := SignedSaturate(b1)
				//...
                //r15 := SignedSaturate(b7

				p0_128i= _mm_packus_epi16 (r1_128i,zero128i);//將8筆16bit的lower bit資料幫裝成16筆8bit的資料
				p1_128i= _mm_packus_epi16 (rc_128i,zero128i);//將8筆16bit的higher bit資料幫裝成16筆8bit的資料
								 
				//取出p8~p0的signed bit組成一個8bit的數值，共16個
				for(numidx=0;numidx<8;numidx++)
				{
					//p0_128i的資料排列如下 P7[7..0] P6[7..0] P5[7..0]..... P0[7..0]，每次取出8個bit如p7[7]p6[7]p5[7]......p0[7],...................,p7[0]p6[0]p5[0].......p0[0] 
				    r0_128i = _mm_slli_epi16(p0_128i,numidx);
					value=_mm_movemask_epi8(r0_128i);									 
					result[value]++;  //Increase histogram bin value
                                        printf("%d\n",value);
					//p1_128i的資料排列如下 P7[15..8] P6[15..8] P5[15..8]..... P0[15..8]，每次取出8個bit如p7[15]p6[15]p5[15]......p0[15],...................,p7[8]p6[8]p5[8].......p0[8] 
					r1_128i = _mm_slli_epi16(p1_128i,numidx);
					value=_mm_movemask_epi8(r1_128i);
					result[value]++;  //Increase histogram bin value
					printf("%d\n",value);
								
				 }
				 */
			
			//Modify ,Jeff Yang, 2010/10/11
			//for(numidx=0;numidx<16;numidx++)
			numidx=0;
			while(numidx<16)
			{
				value = ((sigp[0]>>numidx) & 0x1) | (((sigp[1]>>numidx) & 0x1) << 1) | (((sigp[2]>>numidx) & 0x1) << 2) | (((sigp[3]>>numidx) & 0x1) << 3)
					    | (((sigp[4]>>numidx) & 0x1) << 4) | (((sigp[5]>>numidx) & 0x1) << 5) | (((sigp[6]>>numidx) & 0x1) << 6) | (((sigp[7]>>numidx) & 0x1) << 7);
				result[value]++;  //Increase histogram bin value 
				++numidx;
					 
				value = ((sigp[0]>>numidx) & 0x1) | (((sigp[1]>>numidx) & 0x1) << 1) | (((sigp[2]>>numidx) & 0x1) << 2) | (((sigp[3]>>numidx) & 0x1) << 3)
					    | (((sigp[4]>>numidx) & 0x1) << 4) | (((sigp[5]>>numidx) & 0x1) << 5) | (((sigp[6]>>numidx) & 0x1) << 6) | (((sigp[7]>>numidx) & 0x1) << 7);
				result[value]++;  //Increase histogram bin value 
				++numidx;
			}
			offset+=16;
		}
		++r;
	}
                  
}

//========================================================================
//Camera_Tamper_Running
//
//Input:
//	HandleCT *Handle
//	unsigned char* pSourceGrayImage	-> input source image(Gray)
//Output:
//	none
//Return :
//	 1 Camera Tamper
//	 0 NORMALLY
//Description :
//    Input HandleCT *Handle include image's Height and Width now,
//    pSourceGrayImage from input source image(Gray).
//    That will calculate the LBP histogram(3 by 3 window) of pSourceGrayImage.
//    Warning per difference the LBP histogram of frame at present between STORE_FRAMES frame ago
//    and CAMERA_TAMPER_THRESHOLD.
//======================================================================== 
int Camera_Tamper_Running( HandleCT *Handle , unsigned char* pSourceGrayImage )
{
	int i;
	unsigned int LongTermResultValue,ShortTermResultValue;
	unsigned char Warning=0;	

#if !SHORT_LONG_TERM_TYPE_CHOICE

	if(Handle->NonFristStrat)
	{
		ShortTermResultValue = LongTermResultValue = 0;

		LBP_Histogram( Handle, pSourceGrayImage, Handle->StoreFramesResult[Handle->FrameCount].Result );

		i=1;
		while(i<254)
		{ // must be even 
			LongTermResultValue  +=  abs(Handle->StoreFramesResult[Handle->LongTermFrameCount].Result[i] -
					Handle->StoreFramesResult[Handle->FrameCount].Result[i]);
			ShortTermResultValue +=  abs(Handle->StoreFramesResult[Handle->ShortTermFrameCount].Result[i] -
					Handle->StoreFramesResult[Handle->FrameCount].Result[i]);
			++i;
			LongTermResultValue  +=  abs(Handle->StoreFramesResult[Handle->LongTermFrameCount].Result[i] -
					Handle->StoreFramesResult[Handle->FrameCount].Result[i]);
			ShortTermResultValue +=  abs(Handle->StoreFramesResult[Handle->ShortTermFrameCount].Result[i] -
					Handle->StoreFramesResult[Handle->FrameCount].Result[i]);
			++i;
		}
		//CAMERA TAMPER DETECTED
		if( (LongTermResultValue > Handle->Param.CAMERA_TAMPER_THRESHOLD &&
					ShortTermResultValue >  Handle->Param.CAMERA_TAMPER_THRESHOLD) )
		{

			if(Handle->WarningCount ==  Handle->Param.CAMERA_TAMPER_WARNING_COUNT )
			{
				Handle->ReleaseCount =  Handle->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT;
				Warning = 1;
			}

			Handle->WarningCount = Handle->WarningCount >=  Handle->Param.CAMERA_TAMPER_WARNING_COUNT  ?
				Handle->Param.CAMERA_TAMPER_WARNING_COUNT  : ++Handle->WarningCount;

		}
		else// NORMALLY 
		{
			if(Handle->ReleaseCount > 0)
			{	
				--Handle->ReleaseCount;
				Warning = 1;
			}
			else
			{
				Handle->WarningCount = 0;
				Handle->FrameCount = Handle->LongTermFrameCount;
				Handle->LongTermFrameCount = Handle->LongTermFrameCount <  Handle->Param.STORE_FRAMES ? Handle->LongTermFrameCount+1 : 0;
				Handle->ShortTermFrameCount = Handle->ShortTermFrameCount <  Handle->Param.STORE_FRAMES ? Handle->ShortTermFrameCount+1 : 0;
				Handle->WarningTurnOffCount = 0;
			}
		}
	}
	else
	{
		LBP_Histogram(Handle, pSourceGrayImage, Handle->StoreFramesResult[Handle->FrameCount].Result );

		Handle->FrameCount++;

		if(  Handle->FrameCount  ==   Handle->Param.STORE_FRAMES   )
		{
			Handle->NonFristStrat = 1;
			Handle->LongTermFrameCount = 0;
			Handle->ShortTermFrameCount =  Handle->Param.STORE_FRAMES -  Handle->Param.SHORT_TERM_FRAMES;
		}

	}
	//---------------------2010/09/01
	if(Warning)
	{
		//---------------------2010/09/01
#if (AUTO_MANUAL_WARNINGTURNOFF==0)

		/*Handle->WarningTurnOffCount accumulated when keep WARNING, or 0*/
		Handle->WarningTurnOffCount =
			Handle->ReleaseCount ==  Handle->Param.CAMERA_TAMPER_WARNING_RELEASE_COUNT? ++Handle->WarningTurnOffCount:0;

		if(Handle->WarningTurnOffCount>= Handle->Param.CAMERA_TAMPER_WARNINGTURNOFF_COUNT || Handle->WarningTurnOffManual == 1)
		{
			Handle->WarningTurnOffManual =
			Handle->NonFristStrat  =
			Handle->WarningTurnOffCount =
			Handle->FrameCount =
			Handle->ReleaseCount =
			Handle->WarningCount = 0;
		}
#elif (AUTO_MANUAL_WARNINGTURNOFF==1)

		if( Handle->WarningTurnOffManual )
		{
			Handle->WarningTurnOffManual =
			Handle->NonFristStrat  =
			Handle->WarningTurnOffCount =
			Handle->FrameCount =
			Handle->ReleaseCount =
			Handle->WarningCount = 0;

		}
#endif

		return 1;
	}
	else
	{
		return 0;
	}
	//---------------------
#else

#endif
}

//========================================================================
//	Camera_Tamper_Resetting
//
//		Input:
//			none
//		Output:
//			none
//		Return :
//			none
//		Description :
//			    If parameters or image's resolution are changed would call this function again to
//			    reset related parameters.
//======================================================================== 
void Camera_Tamper_Resetting(HandleCT *Handle)
{
	Handle->ShortTermFrameCount = Handle->LongTermFrameCount = Handle->FrameCount = 0;
	Handle->NonFristStrat = 0;
	Handle->ReleaseCount = Handle->WarningCount = 0;

#if   SHORT_LONG_TERM_TYPE_CHOICE

	Handle->ShortToLongIntervalFramesCount = 0;
	LONG_TERM_FRAMES =  Handle->Param.STORE_FRAMES -  Handle->Param.SHORT_TERM_FRAMES;
#endif
}
