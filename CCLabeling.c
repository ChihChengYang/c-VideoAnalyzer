/*
 ============================================================================
 Name        : CCLabeling.c
 Author      : Jeff Yang
 Version     :
 Copyright   : Jeff Yang
 Description :
 ============================================================================
 */

#include "CCLabeling.h"
#include <xmmintrin.h>
#include <emmintrin.h>

int CCLabeling_Parameters( CCL_Handel *ccl_handle, CCL_Parameters *CCLParameters )
{
	if( CCLParameters->AREA_THRESHOLD <=0  )
	{
		printf("ERROR------ %s %d -------\n",__FILE__, __LINE__ );
		return -1;
	}
	else
	{
		ccl_handle->Param.AREA_THRESHOLD = CCLParameters->AREA_THRESHOLD;
	}

	return 0;
}

int CCLabeling_Create( CCL_Handel **ccl_handle, const int Width, const int Height)
{

	*ccl_handle = (CCL_Handel *) malloc( sizeof(CCL_Handel));
	if( *ccl_handle  ==  0)
		return -1;
   //-------------------------------------------------------------------------------------------------
	(*ccl_handle)->Param.AREA_THRESHOLD = 50;
	//-------------------------------------------------------------------------------------------------
	
	(*ccl_handle)->width =  Width+2;
	(*ccl_handle)->height =  Height+2;
	(*ccl_handle)->height_width = (*ccl_handle)->width * (*ccl_handle)->height;
    (*ccl_handle)->bitmap = (unsigned char *) _mm_malloc( sizeof(unsigned char) * (*ccl_handle)->height_width,16);
	
	if( (*ccl_handle)->bitmap  ==  0)
		return -1;

	(*ccl_handle)->labelmap = (int *) _mm_malloc( sizeof(int) * (*ccl_handle)->height_width,16);
	if( (*ccl_handle)->labelmap  ==  0)
		return -1;

#if CCL_OPTION
	(*ccl_handle)->contourmap = (int *) _mm_malloc( sizeof(int) * (*ccl_handle)->height_width,16);
	if( (*ccl_handle)->labelmap  ==  0)
		return -1;

	memset( (*ccl_handle)->contourmap, 0, sizeof(int) * (*ccl_handle)->height_width );
#endif

	memset((*ccl_handle)->bitmap, 0, sizeof(unsigned char) * (*ccl_handle)->height_width);
	memset((*ccl_handle)->labelmap, 0, sizeof(int) * (*ccl_handle)->height_width );

	return 0;
}

int CCLabeling_Destroy( CCL_Handel *ccl_handle )
{
	_mm_free(ccl_handle->bitmap);
		ccl_handle->bitmap = 0;
	_mm_free(ccl_handle->labelmap);
		ccl_handle->labelmap = 0;
#if CCL_OPTION
	_mm__free(ccl_handle->contourmap);
	ccl_handle->contourmap = 0;
#endif
	free(ccl_handle);
		ccl_handle = 0;

	return 0;
}


// 向7個方向(由該點右方開始，順時針)找是否有連接，
// 有 : (x,y)坐標移過去，break
// 無 : 在labmap上記錄為-1，並開始下一方向的tracer
void CCL_Tracer(CCL_Handel *ccl_handle, int *cy, int *cx, int *tracingdirection )
{
	int i, y, x ,temp;

	for(i = 0; i < 7; i++)
	{
		y = *cy + gCCL_SearchDirection[*tracingdirection][0];
		x = *cx + gCCL_SearchDirection[*tracingdirection][1];

		temp =  y * ccl_handle->width + x;
		if( ccl_handle->bitmap[temp] == 0) //no connection
		{
     		ccl_handle->labelmap[temp] = -1;
			*tracingdirection = (*tracingdirection + 1) & 7; //&7是確定取0~7
		}
		else //connection
		{
			*cy = y;
			*cx = x;
			break;
		}
	}
}

void CCL_ContourTracing(CCL_Handel *ccl_handle, int cy, int cx, int labelindex, int tracingdirection, CCL_Component_Rectangle *ComponentRectangle )
{
	char tracingstopflag = 0, SearchAgain = 1;
	int fx, fy, sx = cx, sy = cy;

#if CCL_RECTANGLE
 	int  labelindex_temp = labelindex-1 ,y_temp,x_temp;
#endif

	CCL_Tracer(ccl_handle, &cy, &cx, &tracingdirection  ); 
	//找到下一個connect的點，若有connect的點A，A坐標為(m,n)，則cx=m, cy=n
	//並記錄找到的方向

#if CCL_OPTION
	ccl_handle->contourmap[cy * ccl_handle->width + cx] = labelindex;
#endif

	if(cx != sx || cy != sy) //有找到connect點時，才會跑下去，否則跳回原caller.
	{
		fx = cx; fy = cy;

		while(SearchAgain)
		{			
			tracingdirection = (tracingdirection + 6) & 7;
			ccl_handle->labelmap[cy * ccl_handle->width + cx] = labelindex;
			CCL_Tracer(ccl_handle, &cy, &cx, &tracingdirection);
			#if CCL_OPTION
				ccl_handle->contourmap[cy * ccl_handle->width + cx] = labelindex;
			#endif
			#if CCL_RECTANGLE
				x_temp = cx-1;
				y_temp = cy-1;

				if( ComponentRectangle[labelindex_temp].nLeftTopY > y_temp)
					ComponentRectangle[labelindex_temp].nLeftTopY = y_temp;     //min( CR[labelindex_temp].LT_y, y_temp )

				if(ComponentRectangle[labelindex_temp].nLeftTopX > x_temp)
					ComponentRectangle[labelindex_temp].nLeftTopX = x_temp;     //min( CR[labelindex_temp].LT_x, x_temp )

				if( ComponentRectangle[labelindex_temp].nRightBottomY < y_temp)
					ComponentRectangle[labelindex_temp].nRightBottomY = y_temp; //MAX( CR[labelindex_temp].LT_y, y_temp )

				if( ComponentRectangle[labelindex_temp].nRightBottomX < x_temp)
					ComponentRectangle[labelindex_temp].nRightBottomX = x_temp; //MAX( CR[labelindex_temp].LT_x, x_temp )
			#endif

			if(cx == sx && cy == sy){
				tracingstopflag = 1;
			}
			else if(tracingstopflag){
				if(cx == fx && cy == fy)
					SearchAgain = 0;
				else
					tracingstopflag = 0;
			} //end of if
		}     //end of while
	}         //end of if(cx != sx || cy != sy)
}

void CCLabeling_Area_Filter( CCL_Handel *ccl_handle, unsigned short *ConnectedComponentsCount)
{
	int x,y;
	y = (*ConnectedComponentsCount)-1;
	x = 0;

	while(x <= y)
	{
		if(ccl_handle->CCLComponentRectangle[x].Area < ccl_handle->Param.AREA_THRESHOLD)
		{
			//Modified, 03/03/2011
			//while( ccl_handle->CCLComponentRectangle[y].Area < ccl_handle->Param.AREA_THRESHOLD && y>=0  &&  x <= y  )
			while(ccl_handle->CCLComponentRectangle[y].Area<ccl_handle->Param.AREA_THRESHOLD && x<=y)
			{
				y--;
				//(*ConnectedComponentsCount)--;
				//Added, 03/03/2011
				if(y < 0)
					break;
			}
			
			if(y >= 0)
			{
				if(ccl_handle->CCLComponentRectangle[y].Area>=ccl_handle->Param.AREA_THRESHOLD && x<=y)
				{
					ccl_handle->CCLComponentRectangle[x] = ccl_handle->CCLComponentRectangle[y];
					y--;
					//(*ConnectedComponentsCount)--;
				}
			}
		}
		x++;
	}//end of while (x <= y)
 
	*ConnectedComponentsCount = y + 1;
}

/*
 * Return Values
 * 0 : Success
 * >0 : ConnectedComponentsCount
 * <0 : Failed
 */
int CCLabeling_Running(CCL_Handel *ccl_handle,  unsigned char *Frame, const int  width, const int height, CCL_Component_Rectangle *ComponentRectangle, unsigned short *ConnectedComponentsCount)
{
	int x, y , tracingdirection,  labelindex, r, k;
 	int temp , temp0 ,labelindex_temp;
	//int flag=0, tmp1;
	//__m128i temp1, temp2, limit, addone;
	__m128i temp1, limit, addone;
	//unsigned char *image_tmp;

 //--------------------------------------------------------------------------------------
 	(*ConnectedComponentsCount) = 0;

	//labelmap initialize
	memset(ccl_handle->labelmap , 0,  sizeof(int)* ccl_handle->height_width );

	//image_tmp  =   (unsigned char*)  malloc(sizeof(unsigned char) * (height+2) * (width+2) * 3   );

	//NOTE !! ccl_handle -> width  = width  + 2;   
	//        ccl_handle -> height = height + 2;
	//即bitmap比原來FG iamge ，上下左右各多一行空白.

	/*===============
	 *Original C code
	 ===============*/
	 /*
	for(  y = 1; y <=  height ; y++)// "<=  height" = "< ccl_handle->height-1"
	{	
		temp0 = y * ccl_handle->width;
		labelindex = (y-1)*width;
		for(  x = 1; x <= width ; x++)
		{
			temp = temp0 + x;
			ccl_handle->bitmap[temp] = Frame[labelindex + (x-1)] < 127 ? 0 : 1; // 1: white image_d[(y-1)*width + (x-1) ] > 127 ? 0 : 1;
		}
	}*/
	/*end of C Code*/

	/*=======================
	 *Using SSE2-epi2 method
	 =======================*/
	r=k=0;
	limit  = _mm_set1_epi8(0);
	addone = _mm_set1_epi8(1);
	for(  y = 1; y <=  height ; y++)// "<=  height" = "< ccl_handle->height-1"
	{	
		for(  x = 0; x < width ; x+=16)
		{	
			temp1 = _mm_loadu_si128(Frame+r);
			
			temp1 = _mm_cmplt_epi8(temp1, limit); //255 in __m128i temp1 would be set as -1(signed char)
			temp1 = _mm_and_si128(temp1, addone );

			_mm_storeu_si128((ccl_handle->bitmap)+k+1+ccl_handle->width, temp1);
	
			r+=16;
			k+=16;
			if (x==(width-16)) {
				k+=2;	
			}
		}
	}
	/*end of SSE2 code*/ 
#if CCL_OPTION
	memset( ccl_handle->contourmap, 0, sizeof(int) * ccl_handle->height_width );
#endif 
	for( y = 1;   y <=  height; ++y)
	{
		temp0 = y * ccl_handle->width;
		for(x = 1, labelindex = 0; x <= width ; ++x)
		{
			temp = temp0 + x;
			if( ccl_handle->bitmap[temp] == 1)//255 )// white pixel
			{				
				if(labelindex != 0)// use pre-pixel label
				{
					ccl_handle->labelmap[temp] = labelindex;
					//---------------
					#if CCL_RECTANGLE
 						ccl_handle->CCLComponentRectangle[labelindex-1].Area++;
					#endif
					 //	Frame[(y-1) * width  + (x-1)] = 125;
					//---------------
				}
				else
				{
					labelindex = ccl_handle->labelmap[temp];
					if( labelindex == 0)
					{
						//----------------------------------
						#if CCL_RECTANGLE
							if((*ConnectedComponentsCount) == ((MAX_RECTANGLE_AMOUNT)-1) )
							{
								#if CCL_AREA_FILTER
									CCLabeling_Area_Filter( ccl_handle, ConnectedComponentsCount);
								#endif
							
								if((*ConnectedComponentsCount) >= MAX_OBJECT_AMOUNT)
										(*ConnectedComponentsCount) = MAX_OBJECT_AMOUNT - 1;
								
								memcpy(ComponentRectangle , ccl_handle->CCLComponentRectangle , (*ConnectedComponentsCount) * sizeof(CCL_Component_Rectangle));
								
								return  (*ConnectedComponentsCount);
							}

							labelindex = ++(*ConnectedComponentsCount);

							labelindex_temp = labelindex-1;
							ccl_handle->CCLComponentRectangle[labelindex_temp].nLeftTopX =
							ccl_handle->CCLComponentRectangle[labelindex_temp].nRightBottomX = x-1;
							ccl_handle->CCLComponentRectangle[labelindex_temp].nLeftTopY =
							ccl_handle->CCLComponentRectangle[labelindex_temp].nRightBottomY = y-1;
							ccl_handle->CCLComponentRectangle[labelindex_temp].Area = 1;
							ccl_handle->CCLComponentRectangle[labelindex_temp].unObjectID = labelindex_temp;
 
						#endif
						//----------------------------------

						tracingdirection = 0;
						ccl_handle->labelmap[temp] = labelindex;
						CCL_ContourTracing(ccl_handle, y, x, labelindex, tracingdirection, ccl_handle->CCLComponentRectangle);// external contour
						
					}
					#if CCL_RECTANGLE
					else{ 
 					    ccl_handle->CCLComponentRectangle[labelindex-1].Area++;
	                }
					#endif
				}
			}    //end of if( ccl_handle->bitmap[temp] == 1) 

			else if(labelindex != 0)// black pixel & pre-pixel has been labeled
			{				
				if( ccl_handle->labelmap[temp] == 0)
				{
					tracingdirection = 1;
					CCL_ContourTracing(ccl_handle, y, x - 1, labelindex, tracingdirection, ccl_handle->CCLComponentRectangle  );// internal contour
				}
				labelindex = 0;
			}
		} //end of x loop
	}     //end of y loop
 
	//----------------------
	#if CCL_AREA_FILTER 
		CCLabeling_Area_Filter( ccl_handle, ConnectedComponentsCount);
	#endif
 
		if((*ConnectedComponentsCount) >= MAX_OBJECT_AMOUNT)
		{
			(*ConnectedComponentsCount) = MAX_OBJECT_AMOUNT - 1;
			memcpy(ComponentRectangle , ccl_handle->CCLComponentRectangle , (*ConnectedComponentsCount) * sizeof(CCL_Component_Rectangle));
		}
		else
		{
			memcpy(ComponentRectangle , ccl_handle->CCLComponentRectangle , (*ConnectedComponentsCount) * sizeof(CCL_Component_Rectangle));
		}
	//----------------------
	return 0;
}
 
void Find_X(int *LT_x, int *RB_x, int sx)
{
	if ( *LT_x > sx ) 
		*LT_x = sx;
	if ( *RB_x < sx )
		*RB_x = sx;
}
 
void Find_Y(int *LT_y, int *RB_y, int sy)
{
	if ( *LT_y > sy ) 
		*LT_y = sy;
	if ( *RB_y < sy )
		*RB_y = sy;
}
 
//========================================================================
// 	Draw Rectangle
// 	Input:
// 		unsigned char* image_s
// 		const int width
// 		const int height
// 		int* XY
//
// 		const int R
// 		const int G
// 		const int B
// 	Output:
// 		unsigned char* image_s
//========================================================================
void  CCLabeling_Draw_Rectangle( unsigned char* image_s , const int width , const int height, CCL_Component_Rectangle *XY, int CCL_Amount,
								const int R, const int G, const int B, unsigned char BMP)
{
	int temp_image ,i,j ;
	int nLeftTopY,nRightBottomY;
	for(i=0; i<CCL_Amount ;++i)
	{
		if(BMP)
		{
			nLeftTopY = ((height-1) - XY[i].nLeftTopY );
			nRightBottomY = ((height-1) - XY[i].nRightBottomY );
		}
		else
		{
			nLeftTopY = (  XY[i].nLeftTopY );
			nRightBottomY = ( XY[i].nRightBottomY );
		}

		//-------------------------------------------
		//                            橫線
		//-------------------------------------------
		for(  j=XY[i].nLeftTopX  ;j<=XY[i].nRightBottomX  ;j++)
		{
			temp_image = 3*( ( nLeftTopY   ) * width + j  );
			image_s[temp_image] = B; image_s[temp_image+1] = G; image_s[temp_image+2] = R;
			temp_image = 3*( ( nRightBottomY   ) * width + j );
			image_s[temp_image] = B; image_s[temp_image+1] = G; image_s[temp_image+2] = R;
		}
		//-------------------------------------------
		//                            直線
		//-------------------------------------------
		for(  j=nRightBottomY   ; j<= nLeftTopY   ; j++)
		{
			temp_image = 3*( ( j ) * width + (XY[i].nLeftTopX   ) );
			image_s[temp_image] = B; image_s[temp_image+1] = G; image_s[temp_image+2] = R;
			temp_image = 3*( ( j ) * width + (XY[i].nRightBottomX    ) );
			image_s[temp_image] = B; image_s[temp_image+1] = G; image_s[temp_image+2] = R;
		}
 
	}

}

//========================================================================
// 	Draw Rectangle
//
// 	Gray Space
//
// 	Input:
// 		unsigned char* image_s
// 		const int width
// 		const int height
// 		int* XY
//		int CCL_Amount
// 		const int Y
// 	Output:
// 		unsigned char* image_s
//========================================================================
void  CCLabeling_Draw_Rectangle_Y( unsigned char* image_s , const int width , const int height, CCL_Component_Rectangle *XY, int CCL_Amount, const int Y  )
{
	int temp_image ,i,j,nLeftTopY,nRightBottomY ;
	for(i=0; i<CCL_Amount ;++i)
	{
		nLeftTopY = ( XY[i].nRightBottomY  );
		nRightBottomY = ( XY[i].nLeftTopY );
		//-------------------------------------------
		//                            橫線
		//-------------------------------------------
		for(  j=XY[i].nLeftTopX  ;j<=XY[i].nRightBottomX  ;j++)
		{
			temp_image =  ( ( nLeftTopY   ) * width + j  );
			image_s[temp_image] = Y;
			temp_image =  ( ( nRightBottomY   ) * width + j );
			image_s[temp_image] = Y;
		}
		//-------------------------------------------
		//                            直線
		//-------------------------------------------
		for(  j=nRightBottomY   ; j<= nLeftTopY    ; j++)
		{
			temp_image =  ( ( j ) * width + (XY[i].nLeftTopX   ) );
			image_s[temp_image] = Y;
			temp_image =  ( ( j ) * width + (XY[i].nRightBottomX    ) );
			image_s[temp_image] = Y;
		}

	}

}
	
//========================================================================
//
//========================================================================
void  CCLabeling_Draw_FullInRectangle_Y( unsigned char* image_s , const int width , const int height, CCL_Component_Rectangle *XY, const int Y  )
{
	int temp_image ,i,j ;
	for(  i=XY[0].nLeftTopY; i<=XY[0].nRightBottomY    ; i++)
	{	temp_image =  ( i * width );
		for(  j=XY[0].nLeftTopX  ;j<=XY[0].nRightBottomX  ;j++)
		{
			image_s[temp_image+j] = Y;
		}
	}
} 
