/*
 * CCLabeling.h
 *
 *  Created on:  
 *      Author: Jeff Yang
 */

#ifndef CCLABELING_H_
#define CCLABELING_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "IVAI_Parameters.h"
//-------------------------------------------------
#define CCL_RECTANGLE 1 // 1:record RECTANGLE   0: NO record RECTANGLE 
//------------------------------------------------
#define CCL_AREA_FILTER 1 // 1:record RECTANGLE   0: NO record RECTANGLE- 
//-------------------------------------------------

#define CCL_OPTION 0 // 1:record contour map   0: NO record contour map

static int gCCL_SearchDirection[8][2] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};

#if CCL_RECTANGLE 
typedef objects_points CCL_Component_Rectangle;
#endif

typedef struct CCL_Handel
{
	unsigned char *bitmap;
	int *labelmap;

#if CCL_OPTION
	int *contourmap;
#endif
    CCL_Component_Rectangle CCLComponentRectangle[MAX_RECTANGLE_AMOUNT];
 	unsigned int width;
	unsigned int height;
	unsigned int height_width;
	unsigned int ConnectedComponentsCount;
 
	CCL_Parameters Param;

} CCL_Handel;
//-----------------

int CCLabeling_Create (CCL_Handel **ccl_handle, const int Width, const int Height);

int CCLabeling_Destroy(CCL_Handel *ccl_handle);
 
int CCLabeling_Running(CCL_Handel *ccl_handle,  unsigned char *Frame, const int  width, const int height,
		CCL_Component_Rectangle *ComponentRectangle, unsigned short *ConnectedComponentsCount);
 
int CCLabeling_Parameters( CCL_Handel *ccl_handle, CCL_Parameters *CCLParameters );
 
int CCL_Test(CCL_Handel *ccl_handle, unsigned char *Frame, const int  width, const int height,  unsigned char *Frame_label);

void  CCLabeling_Draw_Rectangle( unsigned char* image_s , const int width , const int height, CCL_Component_Rectangle *XY, int CCL_Amount,
		const int R, const int G, const int B, unsigned char BMP);

void  CCLabeling_Draw_Rectangle_Y( unsigned char* image_s , const int width , const int height, CCL_Component_Rectangle *XY, int CCL_Amount,
		const int Y  );

void  CCLabeling_Draw_FullInRectangle_Y( unsigned char* image_s , const int width , const int height, CCL_Component_Rectangle *XY, const int Y  );

#endif /* CCLABELING_H_ */
