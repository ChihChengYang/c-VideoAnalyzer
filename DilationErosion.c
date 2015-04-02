/*
 ============================================================================
 Name        : DilationErosion.c
 Author      : Jeff Yang
 Version     :
 Copyright   : Jeff Yang
 Description :  
 ============================================================================
 */

#include "DilationErosion.h"
 

#define LOOP_UNROLL_MORE( CODE ) \
	CODE \
	CODE \
	CODE \
	CODE \
	CODE \
	CODE \
	CODE \
	CODE

/* should always be 2 fewer unrolls than LOOP_UNROLL_MORE */
#define LOOP_UNROLL_MORE2( CODE ) \
	CODE \
	CODE \
	CODE \
	CODE \
	CODE \
	CODE
 
/*
* Morphological erosion with a structuring element 3 pixels square
*
*/
void RegionErode33 (unsigned char  *inoutImg, const int  width, const int  height,const unsigned char mark)
{
 
	const unsigned int   offset = width + 1;
	unsigned char accum[160];  /* 3 pixel tall binary image window for entire row */
	const unsigned char *endImg = inoutImg + width * height;
	unsigned char       *ptrImg = inoutImg;
	unsigned char       *endRow;
	unsigned char *ptrAccum0, *ptrAccum1, *ptrAccum2;

	memset(accum, 0, sizeof(unsigned char) * width);
	/* main loop */
	while (ptrImg != endImg)
	{
		endRow = ptrImg + width;

		ptrAccum0 = accum;
		ptrAccum1 = accum + 1;
		ptrAccum2 = accum + 2;

		/* 0th column in row */
		*ptrAccum0 <<= 1;
		*ptrAccum0 |= (*ptrImg++ != 0);

		/* 1st column in row */
		*ptrAccum1 <<= 1;
		*ptrAccum1 |= (*ptrImg++ != 0);

		/* 2nd through 7th columns in row */
		LOOP_UNROLL_MORE2(
			*ptrAccum2 <<= 1;
		*ptrAccum2 |= (*ptrImg != 0);

		if ( *ptrAccum1 & 0x2
			&& (~(*ptrAccum1) & 0x7
			|| ~(*ptrAccum0) & 0x7
			|| ~(*ptrAccum2) & 0x7) )
		{
			*(ptrImg - offset) = mark;
		}

		ptrImg++;
		ptrAccum0++;
		ptrAccum1++;
		ptrAccum2++;
		)

			while (ptrImg != endRow)
			{
				LOOP_UNROLL_MORE(
					*ptrAccum2 <<= 1;
				*ptrAccum2 |= (*ptrImg != 0);

				if ( *ptrAccum1 & 0x2
					&& (~(*ptrAccum1) & 0x7
					|| ~(*ptrAccum0) & 0x7
					|| ~(*ptrAccum2) & 0x7) )
				{
					*(ptrImg - offset) = mark;
				}

				ptrImg++;
				ptrAccum0++;
				ptrAccum1++;
				ptrAccum2++;
				)
			}
	}
}


/*
* Morphological dilation with a structuring element 3 pixels square
*
*/
void RegionDilate33 (unsigned char  *inoutImg, const int  width, const int  height,const unsigned char mark)
{
	const   unsigned int   offset = width + 1;
	unsigned char accum[160];  /* 3 pixel tall binary image window for entire row */
	const unsigned char *endImg = inoutImg + width *  height;
	unsigned char       *ptrImg = inoutImg;
	unsigned char       *endRow;
	unsigned char *ptrAccum0, *ptrAccum1, *ptrAccum2;
	memset(accum, 0, sizeof(unsigned char) * width);
	/* bounds check for first row */
	endRow = ptrImg + width;
	ptrAccum0 = accum;
	ptrAccum1 = accum + 1;
	ptrAccum2 = accum + 2;
	/* 0th column in row */
	*ptrAccum0 <<= 1;
	*ptrAccum0 |= (*ptrImg++ != 0);
	/* 1st column in row */
	*ptrAccum1 <<= 1;
	*ptrAccum1 |= (*ptrImg++ != 0);
	/* 2nd through 7th columns in row */
	LOOP_UNROLL_MORE2(
		*ptrAccum2 <<= 1;
	*ptrAccum2 |= (*ptrImg != 0);

	if ( ~(*ptrAccum1) & 0x2
		&& (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
	{
		if ( (ptrImg - offset) >= inoutImg )
		{
			*(ptrImg - offset) = mark;
		}
	}

	ptrImg++;
	ptrAccum0++;
	ptrAccum1++;
	ptrAccum2++;
	)
		/* rest of the first row */
		while (ptrImg != endRow)
		{
			LOOP_UNROLL_MORE(
				*ptrAccum2 <<= 1;
			*ptrAccum2 |= (*ptrImg != 0);

			if ( ~(*ptrAccum1) & 0x2
				&& (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
			{
				if ( (ptrImg - offset) >= inoutImg )
				{
					*(ptrImg - offset) = mark;
				}
			}

			ptrImg++;
			ptrAccum0++;
			ptrAccum1++;
			ptrAccum2++;
			)
		}

		/* main loop */
		while (ptrImg != endImg)
		{
			endRow = ptrImg + width;

			ptrAccum0 = accum;
			ptrAccum1 = accum + 1;
			ptrAccum2 = accum + 2;

			/* 0th column in row */
			*ptrAccum0 <<= 1;
			*ptrAccum0 |= (*ptrImg++ != 0);

			/* 1st column in row */
			*ptrAccum1 <<= 1;
			*ptrAccum1 |= (*ptrImg++ != 0);

			/* 2nd through 7th columns in row */
			LOOP_UNROLL_MORE2(
				*ptrAccum2 <<= 1;
			*ptrAccum2 |= (*ptrImg != 0);

			if ( ~(*ptrAccum1) & 0x2
				&& (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
			{
				*(ptrImg - offset) = mark;
			}

			ptrImg++;
			ptrAccum0++;
			ptrAccum1++;
			ptrAccum2++;
			)

				while (ptrImg != endRow)
				{
					LOOP_UNROLL_MORE(
						*ptrAccum2 <<= 1;
					*ptrAccum2 |= (*ptrImg != 0);

					if ( ~(*ptrAccum1) & 0x2
						&& (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
					{
						*(ptrImg - offset) = mark;
					}

					ptrImg++;
					ptrAccum0++;
					ptrAccum1++;
					ptrAccum2++;
					)
				}
		}
}

//=========================================================================================================

void Filter_Erosion2(  unsigned char * ImgS, const int width, const int height , unsigned char * ImgD )
{
	int width_height = sizeof(unsigned char) * width * height;
	int x,y , Offset;

	unsigned char*	P1;	unsigned char*	P2;	unsigned char*	P3;
	unsigned char*	P4;	unsigned char*	P5;	unsigned char*	P6;
	unsigned char*	P7;	unsigned char*	P8;	unsigned char*	P9;
 
	for (  y=height-2; y>=1; --y )
	{
		// 1 2 3
		// 4 5 6
		// 7 8 9
		Offset = y*width + 1; //offset 0 因為取不到1,4,7點的pixel，所以skip最左下的三點(0,575)(1,575)(0,574)由(1,574)開始

		P5 =  (ImgS+Offset);
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;

		for (  x=width-2; x>=2; x-=2)
		{
 			//一次做2個點
			//if P1~P9有任何一點為0, 則ImgD[offset]為0
			//ImgD[Offset] = 255 - (255 - (*P1 & *P2 & *P3 & *P4 & *P5 & *P6 & *P7 & *P8 & *P9) );
			ImgD[Offset] = (*P1 & *P2 & *P3 & *P4 & *P5 & *P6 & *P7 & *P8 & *P9);
 			P1 += 1; P2 += 1; P3 += 1;
			P4 += 1; P5 += 1; P6 += 1;
			P7 += 1; P8 += 1; P9 += 1;
			Offset += 1;

			//ImgD[Offset] = 255 - (255 - (*P1 & *P2 & *P3 & *P4 & *P5 & *P6 & *P7 & *P8 & *P9) );
			ImgD[Offset] = (*P1 & *P2 & *P3 & *P4 & *P5 & *P6 & *P7 & *P8 & *P9);
			P1 += 1; P2 += 1; P3 += 1;
			P4 += 1; P5 += 1; P6 += 1;
			P7 += 1; P8 += 1; P9 += 1;
			Offset += 1;

		}

	}

	memcpy (ImgS,ImgD,width_height);

}

void Filter_Dilation2(  unsigned char * ImgS, const int width , const int height , unsigned char * ImgD )
{
	int width_height = sizeof(unsigned char) * width * height;
	int x,y , Offset;

	unsigned char*	P1;	unsigned char*	P2;	unsigned char*	P3;
	unsigned char*	P4;	unsigned char*	P5;	unsigned char*	P6;
	unsigned char*	P7;	unsigned char*	P8;	unsigned char*	P9;
 
	memset(ImgD , 0,  width_height );

	for (y=height-2; y>=1; --y)
	{
		// 1 2 3
		// 4 5 6
		// 7 8 9
		Offset = y*width + 1;

		P5 = (ImgD+Offset);
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;

		for ( x=width-2; x>=1; x-=2)
		{
			if (ImgS[Offset] != 0)
				*P1 = *P2 =	*P3 = *P4 =	*P5 = *P6 =	*P7 = *P8 =	*P9 = 255;

			P1 += 1; P2 += 1; P3 += 1;
			P4 += 1; P5 += 1; P6 += 1;
			P7 += 1; P8 += 1; P9 += 1;
			Offset += 1;

			if (ImgS[Offset] != 0)
				*P1 = *P2 =	*P3 = *P4 =	*P5 = *P6 =	*P7 = *P8 =	*P9 = 255;

			P1 += 1; P2 += 1; P3 += 1;
			P4 += 1; P5 += 1; P6 += 1;
			P7 += 1; P8 += 1; P9 += 1;
			Offset += 1;
		}
	}
	memcpy (ImgS , ImgD , width_height );
}

/*===================
** Erosion using SSE
 ===================*/
void Filter_Erosion_SSE(  unsigned char * ImgS, const int width , const int height , unsigned char * ImgD )
{
	__m128i tmp1, tmp2, tmp3, tmp4, tmp6, tmp7, tmp8, tmp9,src, dest;
	__m128i and1, and2, and3;
	int width_height = sizeof(unsigned char) * width * height;
	int x,y , Offset, r;

	unsigned char*	P1;	unsigned char*	P2;	unsigned char*	P3;
	unsigned char*	P4;	unsigned char*	P5;	unsigned char*	P6;
	unsigned char*	P7;	unsigned char*	P8;	unsigned char*	P9;

	memset(ImgD , 0,  width_height );
	for (  y=height-2; y>=1; --y )
	{
 		Offset = y*width; //offset 0 因為取不到1,4,7點的pixel，所以skip最左下的三點(0,575)(1,575)(0,574), 而由(1,574)開始
		P5 =  (ImgS+Offset);
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;
		r=0;
 
		for (x=0; x<width ; x+=16)
		{
			src = _mm_loadu_si128((__m128i *)(P5+x)); //load 128bit(=16 elements, 8bit/element) at once
		
			tmp1 = _mm_loadu_si128((__m128i *)(P1+x)); tmp2 = _mm_loadu_si128((__m128i *)(P2+x)); tmp3 = _mm_loadu_si128((__m128i *)(P3+x));
			tmp4 = _mm_loadu_si128((__m128i *)(P4+x));                               tmp6 = _mm_loadu_si128((__m128i *)(P6+x));
			tmp7 = _mm_loadu_si128((__m128i *)(P7+x)); tmp8 = _mm_loadu_si128((__m128i *)(P8+x)); tmp9 = _mm_loadu_si128((__m128i *)(P9+x));

			//AND operator
			and1 = _mm_and_si128(tmp1, _mm_and_si128(tmp4, tmp7));	
			and2 = _mm_and_si128(tmp2, _mm_and_si128(src , tmp8));
			and3 = _mm_and_si128(tmp3, _mm_and_si128(tmp6, tmp9));
			dest = _mm_and_si128(and1, _mm_and_si128(and2, and3));
			//_mm_AND_si128(tmp1, tmp2, tmp3, tmp4, src, tmp6, tmp7, tmp8, tmp9);
		
			//save into ImgD
			_mm_storeu_si128((__m128i*)(ImgD+Offset), dest);	
			
			Offset +=16;	
		}
	}
	memcpy (ImgS,ImgD,width_height);
}
//end of Erosing using SSE

/*===================
** Dilation using SSE
 ===================*/
void Filter_Dilation_SSE(  unsigned char * ImgS, const int width , const int height , unsigned char * ImgD )
{
	my_m128i tmp1, tmp2, tmp3, tmp4, tmp6, tmp7, tmp8, tmp9;
	__m128i OR1, OR2, OR3,src, dest;
	int width_height = sizeof(unsigned char) * width * height;
	int x,y , Offset, r;

	unsigned char*	P1;	unsigned char*	P2;	unsigned char*	P3;
	unsigned char*	P4;	unsigned char*	P5;	unsigned char*	P6;
	unsigned char*	P7;	unsigned char*	P8;	unsigned char*	P9;

	memset(ImgD , 0,  width_height );

	for (  y=height-2; y>=1; y--)
	{
		// 1 2 3
		// 4 5 6
		// 7 8 9
		Offset = y*width ;

		P5 = (ImgS+Offset);
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;
		r=0;	
 
		for ( x=0; x<width; x+=16)
		{
			src = _mm_loadu_si128((__m128i *)(P5+x)); //load 128bit(=16 elements, 8bit/element) at one time
 			tmp1.m1 = _mm_loadu_si128((__m128i *)(P1+x)); tmp2.m1 = _mm_loadu_si128((__m128i *)(P2+x)); tmp3.m1 = _mm_loadu_si128((__m128i *)(P3+x));
			tmp4.m1 = _mm_loadu_si128((__m128i *)(P4+x)); 								  tmp6.m1 = _mm_loadu_si128((__m128i *)(P6+x));
			tmp7.m1 = _mm_loadu_si128((__m128i *)(P7+x)); tmp8.m1 = _mm_loadu_si128((__m128i *)(P8+x)); tmp9.m1 = _mm_loadu_si128((__m128i *)(P9+x));

			//修正在x=0及x=width-16時，解決邊界點問題
			//加入這個fix，以25 frames, 720x576，會增加1.3ms.
			if (x==0) {
				tmp1.m128i_i8[0]=0;
				tmp4.m128i_i8[0]=0;
				tmp7.m128i_i8[0]=0;
			}
			if (x==width-16) {
				tmp3.m128i_i8[15]=0;
				tmp6.m128i_i8[15]=0;
				tmp9.m128i_i8[15]=0;
			}
			if (y == 1) {
				tmp7.m1 = _mm_xor_si128(tmp7.m1, tmp7.m1);
				tmp8.m1 = _mm_xor_si128(tmp8.m1, tmp8.m1);
				tmp9.m1 = _mm_xor_si128(tmp9.m1, tmp9.m1);
			}
			if (y == height-2) {
				tmp1.m1 = _mm_xor_si128(tmp1.m1, tmp1.m1);
				tmp2.m1 = _mm_xor_si128(tmp2.m1, tmp2.m1);
				tmp3.m1 = _mm_xor_si128(tmp3.m1, tmp3.m1);
			}
 
			//OR operation
			OR1 = _mm_or_si128(tmp1.m1, _mm_or_si128(tmp4.m1, tmp7.m1));
			OR2 = _mm_or_si128(tmp2.m1, _mm_or_si128(src , tmp8.m1));
			OR3 = _mm_or_si128(tmp3.m1, _mm_or_si128(tmp6.m1, tmp9.m1));
			dest = _mm_or_si128(OR1, _mm_or_si128(OR2, OR3));			

			//save into ImgD
			_mm_storeu_si128((__m128i*)(ImgD+Offset), dest);

			Offset += 16;
 
		}
	}
	memcpy (ImgS, ImgD, width_height);

}
//end of Filter_Dilation_SSE*/

void Filter_Dilation_self(  unsigned char * ImgS, const int width , const int height , unsigned char * ImgD )
{
	int width_height = sizeof(unsigned char) * width * height;
	int x,y , Offset;

	unsigned char*	P1;	unsigned char*	P2;	unsigned char*	P3;
	unsigned char*	P4;	unsigned char*	P5;	unsigned char*	P6;
	unsigned char*	P7;	unsigned char*	P8;	unsigned char*	P9;
 
	memset(ImgD , 0,  width_height );

	for ( y=height-2; y>=1; y--)
	{
		// 1 2 3
		// 4 5 6
		// 7 8 9
		Offset = y*width + 1;
		P5 = (ImgS+Offset);
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;

		for ( x=width-1; x>=1; x--)
		{
			if (*P1 | *P2 | *P3 | *P4 | *P5 | *P6 | *P7 | *P8 | *P9)
				ImgD[Offset] = 255;

			P1 += 1; P2 += 1; P3 += 1;
			P4 += 1; P5 += 1; P6 += 1;
			P7 += 1; P8 += 1; P9 += 1;
			Offset += 1;
		}
	}
	memcpy (ImgS , ImgD , width_height );
}

void ED_SSE(  unsigned char * ImgS, const int width, const int height, unsigned char * ImgD, unsigned char * image_tmp) 
{
 
	my_m128i tmp1D, tmp2D, tmp3D, tmp4D, tmp6D, tmp7D, tmp8D, tmp9D;
	__m128i tmp1, tmp2, tmp3, tmp4, tmp6, tmp7, tmp8, tmp9,src, dest;
	__m128i and1, and2, and3;
	__m128i OR1, OR2, OR3;
	int width_height = sizeof(unsigned char) * width * height;
 	int x,y , Offset, Offset1, Offset2, i;
 	unsigned char*	P1;	unsigned char*	P2;	unsigned char*	P3;
	unsigned char*	P4;	unsigned char*	P5;	unsigned char*	P6;
	unsigned char*	P7;	unsigned char*	P8;	unsigned char*	P9;

	unsigned char*	S1;	unsigned char*	S2;	unsigned char*	S3;
	unsigned char*	S4;	unsigned char*	S5;	unsigned char*	S6;
	unsigned char*	S7;	unsigned char*	S8;	unsigned char*	S9;
 
	memset(ImgD , 0,  width_height );
	memset(image_tmp , 0,  width_height );

	//Part1: first 4 lines
	for (i=height-2; i>=height-5; i--)
	{
		Offset = i*width ;

		P5 = (ImgS+Offset);
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;
		for (x=0; x<width ; x+=16)
		{
			//---------------------EROSION------------------
			src = _mm_loadu_si128((__m128i *)(P5+x));		
			tmp1 = _mm_loadu_si128((__m128i *)(P1+x)); tmp2 = _mm_loadu_si128((__m128i *)(P2+x)); tmp3 = _mm_loadu_si128((__m128i *)(P3+x));
			tmp4 = _mm_loadu_si128((__m128i *)(P4+x));                               tmp6 = _mm_loadu_si128((__m128i *)(P6+x));
			tmp7 = _mm_loadu_si128((__m128i *)(P7+x)); tmp8 = _mm_loadu_si128((__m128i *)(P8+x)); tmp9 = _mm_loadu_si128((__m128i *)(P9+x));

			and1 = _mm_and_si128(tmp1, _mm_and_si128(tmp4, tmp7));
			and2 = _mm_and_si128(tmp2, _mm_and_si128(src , tmp8));
			and3 = _mm_and_si128(tmp3, _mm_and_si128(tmp6, tmp9));
			dest = _mm_and_si128(and1, _mm_and_si128(and2, and3));
 	
			_mm_storeu_si128((__m128i *)(image_tmp+Offset), dest);	
			Offset +=16;
		}
	}

	//Part2: remainar lines
	for (  y=height-6, i=height-2; y>=1; y--, i--)
	{
		Offset1 = y*width ; 
		Offset2 = i*width;

		P5 = (ImgS+Offset1); //Erosion
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;

		S5 = (image_tmp+Offset2); //Dilation
		S2 = S5 - width;
		S8 = S5 + width;
		S1 = S2 - 1;
		S3 = S2 + 1;
		S4 = S5 - 1;
		S6 = S5 + 1;
		S7 = S8 - 1;
		S9 = S8 + 1;

		for (x=0; x<width ; x+=16)
		{

			//---------------------EROSION------------------
			src = _mm_loadu_si128((__m128i *)(P5+x)); 
			tmp1 = _mm_loadu_si128((__m128i *)(P1+x)); tmp2 = _mm_loadu_si128((__m128i *)(P2+x)); tmp3 = _mm_loadu_si128((__m128i *)(P3+x));
			tmp4 = _mm_loadu_si128((__m128i *)(P4+x));                               tmp6 = _mm_loadu_si128((__m128i *)(P6+x));
			tmp7 = _mm_loadu_si128((__m128i *)(P7+x)); tmp8 = _mm_loadu_si128((__m128i *)(P8+x)); tmp9 = _mm_loadu_si128((__m128i *)(P9+x));

			and1 = _mm_and_si128(tmp1, _mm_and_si128(tmp4, tmp7));
			and2 = _mm_and_si128(tmp2, _mm_and_si128(src , tmp8));
			and3 = _mm_and_si128(tmp3, _mm_and_si128(tmp6, tmp9));
			dest = _mm_and_si128(and1, _mm_and_si128(and2, and3));
 
			_mm_storeu_si128((__m128i *)(image_tmp+Offset1), dest);
			Offset1 +=16;


			//---------------------DILATION------------------
			dest = _mm_xor_si128(dest, dest);
			src = _mm_loadu_si128((__m128i *)(S5+x));		
			tmp1D.m1 = _mm_loadu_si128((__m128i *)(S1+x)); tmp2D.m1 = _mm_loadu_si128((__m128i *)(S2+x)); tmp3D.m1 = _mm_loadu_si128((__m128i *)(S3+x));
			tmp4D.m1 = _mm_loadu_si128((__m128i *)(S4+x));                               tmp6D.m1 = _mm_loadu_si128(S6+x);
			tmp7D.m1 = _mm_loadu_si128((__m128i *)(S7+x)); tmp8D.m1 = _mm_loadu_si128((__m128i *)(S8+x)); tmp9D.m1 = _mm_loadu_si128((__m128i *)(S9+x));

			if (x==0) {
				tmp1D.m128i_i8[0]=0;
				tmp4D.m128i_i8[0]=0;
				tmp7D.m128i_i8[0]=0;
			}
			if (x==width-16) {
				tmp3D.m128i_i8[15]=0;
				tmp6D.m128i_i8[15]=0;
				tmp9D.m128i_i8[15]=0;
			}
			if (i == height-2) {
				tmp1D.m1 = _mm_xor_si128(tmp1D.m1, tmp1D.m1); //把i=height-1 視為0
				tmp2D.m1 = _mm_xor_si128(tmp2D.m1, tmp2D.m1);
				tmp3D.m1 = _mm_xor_si128(tmp3D.m1, tmp3D.m1);
			}

			OR1 = _mm_or_si128(tmp1D.m1, _mm_or_si128(tmp4D.m1, tmp7D.m1));
			OR2 = _mm_or_si128(tmp2D.m1, _mm_or_si128(src , tmp8D.m1));
			OR3 = _mm_or_si128(tmp3D.m1, _mm_or_si128(tmp6D.m1, tmp9D.m1));
			dest = _mm_or_si128(OR1, _mm_or_si128(OR2, OR3));
 
			_mm_storeu_si128((__m128i *)(ImgD+Offset2), dest);			
			Offset2 +=16;
		}
	}

	//Part3: dilation remainar 4 Lines
	for (i=4; i>=1; i--)
	{
		Offset = i*width ;

		P5 = (image_tmp+Offset);
		P2 = P5 - width;
		P8 = P5 + width;
		P1 = P2 - 1;
		P3 = P2 + 1;
		P4 = P5 - 1;
		P6 = P5 + 1;
		P7 = P8 - 1;
		P9 = P8 + 1;
		for (x=0; x<width ; x+=16)
		{
			//---------------------DILATION------------------
			src = _mm_loadu_si128((__m128i *)(P5+x));		
			tmp1D.m1 = _mm_loadu_si128((__m128i *)(P1+x)); tmp2D.m1 = _mm_loadu_si128((__m128i *)(P2+x)); tmp3D.m1 = _mm_loadu_si128((__m128i *)(P3+x));
			tmp4D.m1 = _mm_loadu_si128((__m128i *)(P4+x));                               tmp6D.m1 = _mm_loadu_si128((__m128i *)(P6+x));
			tmp7D.m1 = _mm_loadu_si128((__m128i *)(P7+x)); tmp8D.m1 = _mm_loadu_si128((__m128i *)(P8+x)); tmp9D.m1 = _mm_loadu_si128((__m128i *)(P9+x));

			if (x==0) {
				tmp1D.m128i_i8[0]=0;
				tmp4D.m128i_i8[0]=0;
				tmp7D.m128i_i8[0]=0;
			}
			if (x==width-16) {
				tmp3D.m128i_i8[15]=0;
				tmp6D.m128i_i8[15]=0;
				tmp9D.m128i_i8[15]=0;
			}
			if (i == 1) {
				tmp7D.m1 = _mm_xor_si128(tmp7D.m1, tmp7D.m1);
				tmp8D.m1 = _mm_xor_si128(tmp8D.m1, tmp8D.m1);
				tmp9D.m1 = _mm_xor_si128(tmp9D.m1, tmp9D.m1);
			}

			OR1 = _mm_or_si128(tmp1D.m1, _mm_or_si128(tmp4D.m1, tmp7D.m1));
			OR2 = _mm_or_si128(tmp2D.m1, _mm_or_si128(src , tmp8D.m1));
			OR3 = _mm_or_si128(tmp3D.m1, _mm_or_si128(tmp6D.m1, tmp9D.m1));
			dest = _mm_or_si128(OR1, _mm_or_si128(OR2, OR3));
	
			_mm_storeu_si128((__m128i*)(ImgD+Offset), dest);
			
			Offset +=16;
		}
	}

	memcpy (ImgS,ImgD,width_height);
 
}
