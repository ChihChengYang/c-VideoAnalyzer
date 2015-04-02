//      DilationErosion
#ifndef DILATIONEROSION_H_
#define DILATIONEROSION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xmmintrin.h>
#include <emmintrin.h>

typedef union my_m128i{
        __m128i m1;
		char 	m128i_i8[16];
		short 	m128i_i16[8];
        int 	m128i_i32[4];
		
}my_m128i;

typedef union my_m128{
        __m128 m1;
        float m128_f32[4];
}my_m128;

extern void RegionErode33   (unsigned char *inoutImg, const int  width, const int  height,const unsigned char mark);
extern void RegionDilate33  (unsigned char *inoutImg, const int  width, const int  height,const unsigned char mark);
extern void Filter_Erosion2 (unsigned char * ImgS, const int width, const int height , unsigned char * ImgD);
extern void Filter_Dilation2(unsigned char * ImgS, const int width, const int height , unsigned char * ImgD);

extern void Filter_Erosion_SSE (unsigned char * ImgS, const int width, const int height , unsigned char * ImgD);
extern void Filter_Dilation_SSE(unsigned char * ImgS, const int width, const int height , unsigned char * ImgD);
extern void ED_SSE(unsigned char * ImgS, const int width, const int height , unsigned char * ImgD, unsigned char * image_tmp);

#endif