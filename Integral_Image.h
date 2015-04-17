/*
 * Integral_Image.h
 *
 *  Created on:  
 *      Author:  Jeff Yang
 */

#ifndef INTEGRAL_IMAGE_H_
#define INTEGRAL_IMAGE_H_

 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <math.h>

#define ImageScale_average(a, b)   (unsigned char)(( (int)(a) + (int)(b) ) >> 1)

 //#define round(a)   ( (int)(floor(a + 0.5 )) )
 #define round(a)   ( (int)  (a + 0.5 )  )


 void IntegralImage_IntegralImage_Gray( unsigned char *image_s, const int width  , const int height , int *image_d);

 int IntegralImage_Get_Feature_Position(int* info , int position, int xSize ,int ySize , int LearnerIndex , int* FeatureCounterEnd);

 void ImageScale_ScaleRect( unsigned char *Source, int SrcWidth, int SrcHeight, unsigned char *Target,  int TgtWidth, int TgtHeight);
 
 
#endif /* INTEGRAL_IMAGE_H_ */
