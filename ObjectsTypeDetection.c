/*
 * ObjectsTypeDetection.c
 *
 */

#include "ObjectsTypeDetection.h"


int ObjectsTypeDetection_Parameters( OTD_Handel *otd_handle, OTD_Parameters *OTDParameters )
{
	int i,j,jj,ii;
	float temp;
 
#if(SEQUENTIAL_KMEANS_ONOFF)
	j = 0;
	while( j < otd_handle->objectsSizeDefineCount)
	{
		otd_handle->SKMObjects[j].ObjectsStoreCount = 0;
		++j;
	}
#endif

	otd_handle->objectsSizeDefineCount = 0;

	i = 0;
	while( i < OBJECTS_SIZE_TYPE_DEFINE_AMOUNT)
	{
		if( OTDParameters->ostd[i].height > 0  &&   OTDParameters->ostd[i].width > 0)
		{
			j = otd_handle->objectsSizeDefineCount;
  
#if(SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==1)
			otd_handle->SKMObjects[i].MeanHeight = OTDParameters->ostd[i].height;
			otd_handle->SKMObjects[i].MeanWidth = OTDParameters->ostd[i].width;
#else
			otd_handle->OSD[j].lowerHeight = WIDTH_HEIGHT_RATIO_LOWER * OTDParameters->ostd[i].height;
			otd_handle->OSD[j].lowerWidth =  WIDTH_HEIGHT_RATIO_LOWER * OTDParameters->ostd[i].width;
			otd_handle->OSD[j].upperHeight = WIDTH_HEIGHT_RATIO_UPPER * OTDParameters->ostd[i].height;
			otd_handle->OSD[j].upperWidth =  WIDTH_HEIGHT_RATIO_UPPER * OTDParameters->ostd[i].width;
#if(!ADJUST_UNKNOWN_SIZE_BTWEEN_TWO_TYPE)
			otd_handle->OSD[j].lowerArea =  otd_handle->OSD[j].lowerHeight *  otd_handle->OSD[j].lowerWidth;
			otd_handle->OSD[j].upperArea =  otd_handle->OSD[j].upperHeight *  otd_handle->OSD[j].upperWidth;
#endif

#endif

			otd_handle->OSD[j].OST = OTDParameters->ostd[i].OST;
			++otd_handle->objectsSizeDefineCount;

		}

		++i;
	}
 

#if( ( !SEQUENTIAL_KMEANS_ONOFF || ( SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==0) ) &&  ADJUST_UNKNOWN_SIZE_BTWEEN_TWO_TYPE ) 

	if( otd_handle->objectsSizeDefineCount > 0)
	{
		ii = (int)(OBJECTSSIZETYPE_THEBOTTOM-1);
		for(i = 0; i < ii ; ++i )
		{
			for(j = 0; j < otd_handle->objectsSizeDefineCount ; ++j )
			{
				if(otd_handle->OSD[j].OST == (ObjectsSizeType)i) //tiny,small,...
				{
					for(jj = 0; jj < otd_handle->objectsSizeDefineCount ; ++jj )
					{
						if( otd_handle->OSD[jj].OST == (ObjectsSizeType)(i+1) ) //small,middle,...
						{
							if( otd_handle->OSD[j].upperHeight > 0 )
							{
								temp = (float)(otd_handle->OSD[j].upperHeight + otd_handle->OSD[jj].lowerHeight) * 0.5;
								otd_handle->OSD[jj].lowerHeight = otd_handle->OSD[j].upperHeight = (unsigned int) temp;

							}
							if( otd_handle->OSD[j].upperWidth > 0 )
							{
								temp = (float)(otd_handle->OSD[j].upperWidth + otd_handle->OSD[jj].lowerWidth) * 0.5;
								otd_handle->OSD[jj].lowerWidth = otd_handle->OSD[j].upperWidth = (unsigned int) temp;

							}
						}

					}
				}

			}

		}

		for(j = 0; j < otd_handle->objectsSizeDefineCount ; ++j )
		{
			otd_handle->OSD[j].lowerArea =  otd_handle->OSD[j].lowerHeight *  otd_handle->OSD[j].lowerWidth;
			otd_handle->OSD[j].upperArea =  otd_handle->OSD[j].upperHeight *  otd_handle->OSD[j].upperWidth;
        }


	}

#endif

	return 0;
}

int ObjectsTypeDetection_Create( OTD_Handel **otd_handle, OTD_Parameters *OTD_Param)
{
	int i;
	objects_size_type_define *OSTD;
	objects_size_type_define Def_ostd[OBJECTS_SIZE_TYPE_DEFINE_AMOUNT] = {{30, 30, OBJECTSSIZETYPE_SMALL},
        	              	    				{60, 60, OBJECTSSIZETYPE_MEDIUM},
        	              	   	 			{120, 120, OBJECTSSIZETYPE_LARGE}};

	float widthHeightRatioUpper = 1.4, widthHeightRatioLower = 0.7;

	*otd_handle = (OTD_Handel *) malloc( sizeof(OTD_Handel));
	if( *otd_handle  ==  0)
	{
		printf("ERROR------ %s %d -------\n",__FILE__, __LINE__ );
		return -1;
	}

#if(OBJECTS_COLOR_TYPE_ONOFF)

	if(OBJECTSCOLORTYPE_THEBOTTOM >= 1 )
	{
		(*otd_handle)->ObjectsColorHistogram = (float *) malloc( sizeof(float) *  OBJECTSCOLORTYPE_THEBOTTOM);
		if( (*otd_handle)->ObjectsColorHistogram  ==  0)
		{
			printf("ERROR------ %s %d -------\n",__FILE__, __LINE__ );
			return -1;
		}
	}
	else 
		(*otd_handle)->ObjectsColorHistogram  =  NULL;
#endif 

	(*otd_handle)->objectsSizeDefineCount = OBJECTS_SIZE_TYPE_DEFINE_AMOUNT;

#if(SEQUENTIAL_KMEANS_ONOFF)
 
	(*otd_handle)->SKMObjects = (SequentialKMeansObjects *) malloc( sizeof(SequentialKMeansObjects) * (*otd_handle)->objectsSizeDefineCount);
	if( (*otd_handle)->SKMObjects  ==  0)
	{
		printf("ERROR------ %s %d -------\n",__FILE__, __LINE__ );
		return -1;
	}
	memset((*otd_handle)->SKMObjects, 0, sizeof(SequentialKMeansObjects) * (*otd_handle)->objectsSizeDefineCount);

#endif

	(*otd_handle)->OSD = (ObjectsSizeDefine *) malloc( sizeof(ObjectsSizeDefine) * (*otd_handle)->objectsSizeDefineCount);
	if( (*otd_handle)->OSD  ==  0)
	{
		printf("ERROR------ %s %d -------\n",__FILE__, __LINE__ );
		return -1;
	}
	memset((*otd_handle)->OSD, 0, sizeof(ObjectsSizeDefine) * (*otd_handle)->objectsSizeDefineCount);
 
	OSTD = OTD_Param==NULL ? Def_ostd : OTD_Param->ostd;

	for(i=0;i<(*otd_handle)->objectsSizeDefineCount; ++i)
	{
 		(*otd_handle)->OSD[i].OST = OSTD[i].OST;
 
#if(SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==1)
		(*otd_handle)->SKMObjects[i].MeanHeight = OSTD[i].height;
		(*otd_handle)->SKMObjects[i].MeanWidth = OSTD[i].width;
#else
		(*otd_handle)->OSD[i].lowerHeight = (unsigned int) (widthHeightRatioLower * (float)OSTD[i].height);
		(*otd_handle)->OSD[i].lowerWidth = (unsigned int) (widthHeightRatioLower * (float)OSTD[i].width);
		(*otd_handle)->OSD[i].upperHeight = (unsigned int) (widthHeightRatioUpper * (float)OSTD[i].height);
		(*otd_handle)->OSD[i].upperWidth = (unsigned int) (widthHeightRatioUpper * (float)OSTD[i].width);
		(*otd_handle)->OSD[i].lowerArea =  (*otd_handle)->OSD[i].lowerHeight *  (*otd_handle)->OSD[i].lowerWidth;
		(*otd_handle)->OSD[i].upperArea =  (*otd_handle)->OSD[i].upperHeight *  (*otd_handle)->OSD[i].upperWidth;
#endif
	}


	return 0;
}


int ObjectsTypeDetection_Destroy( OTD_Handel *otd_handle )
{
	free( otd_handle->OSD);
		otd_handle->OSD = 0;

	#if(OBJECTS_COLOR_TYPE_ONOFF)
		free( otd_handle->ObjectsColorHistogram);
			otd_handle->ObjectsColorHistogram = 0;
	#endif

#if(SEQUENTIAL_KMEANS_ONOFF )
	free( otd_handle->SKMObjects);
	otd_handle->SKMObjects = 0;
#endif

	free( otd_handle );
		otd_handle  = 0;

	return 0;
}


#if(SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==0)
// l1 distance
// l2 distance
void Sequential_KMeans2( OTD_Handel *otd_handle ,int j )
{
	int i;
	float SumHeight , SumWidth;


	SumHeight = SumWidth = 0;
	i = 0;
	while( i < otd_handle->SKMObjects[j].ObjectsStoreCount ) 
	{
		SumHeight += (float)otd_handle->SKMObjects[j].ObjectsStore[i].Height;
		SumWidth += (float)otd_handle->SKMObjects[j].ObjectsStore[i].Width;	
		++i;
	}

	otd_handle->SKMObjects[j].MeanHeight = (int) ( SumHeight / (float) otd_handle->SKMObjects[j].ObjectsStoreCount);	
	otd_handle->SKMObjects[j].MeanWidth = (int) ( SumWidth / (float) otd_handle->SKMObjects[j].ObjectsStoreCount);

	otd_handle->OSD[j].lowerHeight = WIDTH_HEIGHT_RATIO_LOWER * otd_handle->SKMObjects[j].MeanHeight;
	otd_handle->OSD[j].lowerWidth =  WIDTH_HEIGHT_RATIO_LOWER * otd_handle->SKMObjects[j].MeanWidth;
	otd_handle->OSD[j].upperHeight = WIDTH_HEIGHT_RATIO_UPPER * otd_handle->SKMObjects[j].MeanHeight;
	otd_handle->OSD[j].upperWidth =  WIDTH_HEIGHT_RATIO_UPPER * otd_handle->SKMObjects[j].MeanWidth;
	otd_handle->OSD[j].lowerArea =  otd_handle->OSD[j].lowerHeight *  otd_handle->OSD[j].lowerWidth;
	otd_handle->OSD[j].upperArea =  otd_handle->OSD[j].upperHeight *  otd_handle->OSD[j].upperWidth;

	otd_handle->SKMObjects[j].ObjectsStoreCount = 0;

}

#endif

#if(SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==1)

void Sequential_KMeans( OTD_Handel *otd_handle, int j  )
{
	int i; 
	float SumHeight , SumWidth, SumDistance, Count;

	SumDistance = 0;
	i = 0;
	while( i < otd_handle->SKMObjects[j].ObjectsStoreCount ) 
	{
		SumDistance += (float)otd_handle->SKMObjects[j].ObjectsStore[i].DistanceBetweenMean;
		++i;
	}
	SumDistance = (int) (SumDistance/(float) otd_handle->SKMObjects[j].ObjectsStoreCount);

	Count = SumHeight = SumWidth = 0;
	i = 0;
	while( i < otd_handle->SKMObjects[j].ObjectsStoreCount ) 
	{
		if( otd_handle->SKMObjects[j].ObjectsStore[i].DistanceBetweenMean <= SumDistance )
		{
			SumHeight += (float)otd_handle->SKMObjects[j].ObjectsStore[i].Height;
			SumWidth += (float)otd_handle->SKMObjects[j].ObjectsStore[i].Width;	
			++Count;
		}
		++i;
	}

	if( Count > 0 )
	{
		otd_handle->SKMObjects[j].MeanHeight = (int) ( SumHeight / Count);	
		otd_handle->SKMObjects[j].MeanWidth = (int) ( SumWidth / Count);
	}

	otd_handle->SKMObjects[j].ObjectsStoreCount = 0;
}

#endif

int ObjectsTypeDetection_Running( OTD_Handel *otd_handle, objects_points *Objects, dd_group *DD_Group, const int ObjectsAmount, IpcEventDataIntelligent_TypeData *pEvent )
{
	int i,j;
	int ObjectArea,Objectheight,Objectwidth;
	TypeInOut pData[MAX_PC_GROUP][OBJECTS_SIZE_TYPE_DEFINE_AMOUNT];
	IpcEventDataIntelligent_TypeData *pOut = pEvent;
	int Find[MAX_PC_GROUP][OBJECTS_SIZE_TYPE_DEFINE_AMOUNT];
	int OTDCount=0;
	
#if(SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==1)	
	int MinDistance, MinObjectsSizeDefine, temp ;
#endif
	
	memset(Find, 0, sizeof(Find));
	memset(pData, 0, sizeof(pData));

	i = 0;
	while( i < ObjectsAmount)
	{

 		Objectheight = ( Objects[i].nRightBottomY - Objects[i].nLeftTopY ) ;
		Objectwidth = ( Objects[i].nRightBottomX - Objects[i].nLeftTopX );

#if(SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==1)
 
		MinDistance = INT_MAX;
		j = 0;
		while( j < otd_handle->objectsSizeDefineCount)
		{
			temp = SQR( Objectheight - otd_handle->SKMObjects[j].MeanHeight) +
				SQR( Objectwidth - otd_handle->SKMObjects[j].MeanWidth);

			if( MinDistance > temp)
			{
				MinDistance = temp;
				MinObjectsSizeDefine = j;
			}

			++j;
		}

		if( MinDistance != INT_MAX)
		{
			temp = (otd_handle->SKMObjects[MinObjectsSizeDefine].ObjectsStoreCount);

			otd_handle->SKMObjects[MinObjectsSizeDefine].ObjectsStore[ temp ].Height
				= Objectheight;
			otd_handle->SKMObjects[MinObjectsSizeDefine].ObjectsStore[ temp ].Width
				= Objectwidth;
			otd_handle->SKMObjects[MinObjectsSizeDefine].ObjectsStore[ temp ].DistanceBetweenMean
				= MinDistance;
			++otd_handle->SKMObjects[MinObjectsSizeDefine].ObjectsStoreCount;

			if( otd_handle->SKMObjects[MinObjectsSizeDefine].ObjectsStoreCount >= SEQUENTIAL_KMEANS_STORE)
				Sequential_KMeans( otd_handle, MinObjectsSizeDefine );

			Objects[i].SizeType = otd_handle->OSD[MinObjectsSizeDefine].OST;

			if(DD_Group[i].InOrOut == 0)	//In
				pData[DD_Group[i].Group][MinObjectsSizeDefine].nIn++;
			else
				pData[DD_Group[i].Group][MinObjectsSizeDefine].nOut++;

			pData[DD_Group[i].Group][MinObjectsSizeDefine].Type = Objects[i].SizeType;
			Find[DD_Group[i].Group][MinObjectsSizeDefine] = 1;

		}

#elif(SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==0)

		j = 0;
		while( j < otd_handle->objectsSizeDefineCount)
		{

			if( otd_handle->OSD[j].lowerHeight <=  Objectheight &&   Objectheight <  otd_handle->OSD[j].upperHeight &&
					otd_handle->OSD[j].lowerWidth <=  Objectwidth &&   Objectwidth <  otd_handle->OSD[j].upperWidth)
			{

				otd_handle->SKMObjects[j].ObjectsStore[ (otd_handle->SKMObjects[j].ObjectsStoreCount) ].Height
					= Objectheight;
				otd_handle->SKMObjects[j].ObjectsStore[ (otd_handle->SKMObjects[j].ObjectsStoreCount) ].Width
					= Objectwidth;
				++otd_handle->SKMObjects[j].ObjectsStoreCount;

				if( otd_handle->SKMObjects[j].ObjectsStoreCount >= SEQUENTIAL_KMEANS_STORE)
					Sequential_KMeans2( otd_handle, j );


				Objects[i].SizeType = otd_handle->OSD[j].OST;

				if(DD_Group[i].InOrOut == 0)	//In
					pData[DD_Group[i].Group][j].nIn++;
				else
					pData[DD_Group[i].Group][j].nOut++;
				pData[DD_Group[i].Group][j].Type = Objects[i].SizeType;
				Find[DD_Group[i].Group][j] = 1;

				break;
			}

			++j;
		}

		if(j == otd_handle->objectsSizeDefineCount)
			Objects[i].SizeType = OBJECTSSIZETYPE_INVALID;
#else
		j = 0;
		while( j < otd_handle->objectsSizeDefineCount)
		{

			if( otd_handle->OSD[j].lowerHeight <=  Objectheight &&   Objectheight <  otd_handle->OSD[j].upperHeight &&
					otd_handle->OSD[j].lowerWidth <=  Objectwidth &&   Objectwidth <  otd_handle->OSD[j].upperWidth)
			{
				Objects[i].SizeType = otd_handle->OSD[j].OST;

				if(DD_Group[i].InOrOut == 0)	//In
					pData[DD_Group[i].Group][j].nIn++;
				else
					pData[DD_Group[i].Group][j].nOut++;
				pData[DD_Group[i].Group][j].Type = Objects[i].SizeType;
				Find[DD_Group[i].Group][j] = 1;

				break;
			}

			++j;
		}

		if(j == otd_handle->objectsSizeDefineCount)
			Objects[i].SizeType = OBJECTSSIZETYPE_INVALID;
#endif


		++i;
	}
 
	for(j=0; j<MAX_PC_GROUP; j++)
	{
		for(i=0; i<otd_handle->objectsSizeDefineCount; i++)
		{
			if(Find[j][i])
			{
				//Group ID | Color Type
				pOut->Group_Type = (j<<4) | pData[j][i].Type;
				pOut->nInOut = (pData[j][i].nIn<<4) | pData[j][i].nOut;
				pOut++;
				OTDCount++;
			}
		}
	}

	return OTDCount;
}


/*---------------------------------------------
//RGB 轉換 HSV 
// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//                if s == 0, then h = -1 (undefined)
//
//   輸入參數 Input:
//      Pic		影像1維陣列 W*H*3
//        W		影像的寬
//        H		影像的高
//        HSV     影像1維陣列 W*H*3
//
//  輸出參數 Output:
//        HSV     影像1維陣列 W*H*3
//       (
//           V (B)	 影像1維陣列  3 * (width * y + x),
//           S (G)	 影像1維陣列  3 * (width * y + x)+1
//           H (R)	 影像1維陣列  3 * (width * y + x)+2
//       )
//			      另一種寫法的公式來表達：
//				  讀入值為RGB先除255換算為0~1之間的小數(其中RGB代表Red,Green,Blue),而HSV是轉換後的值。
//				  MAX為R,G,B中的最大值，MIN為R,G,B中的最小值
//				  H = (0+(G-B)/(MAX-MIN))*60 ,IF R=MAX
//				  H = (2+(B-R)/(MAX-MIN))*60 ,IF G=MAX
//				  H = (4+(R-G)/(MAX-MIN))*60 ,IF B=MAX
//				  S = (MAX-MIN)/MAX         (採用Adobe模式算法，圓柱色立體)
//				  V = MAX
//				  ※其中有些除法，要注意，若(MAX-MIN)=0，表示R=G=B時，則H=0，而且S=0.
//				  ※其中的H若計算後變為負數，則請加上360度！而S,V算好後要再乘100，因為是以%來表示的。
//				  請就以上所提供的公式，設計一個輸入R,G,B，可以輸出H,S,B的程式。
//				  例如(R,G,B)=(240,36,186)則輸出的(H,S,B)=( 316,85,94) 
//
//---------------------------------------------*/
void RGB2HSV( const unsigned char R, const unsigned char G, const unsigned char B, float *HSV )
{
	float min, max, delta;
	float r,g,b;

	r = (float) R/255.0;
	g = (float) G/255.0;
	b = (float) B/255.0;

	min = MIN3( r, g, b );
	max = MAX3( r, g, b );
	HSV[2] = max;        // v

	delta = max - min;

	if( max != 0 )
		HSV[1] = delta / max;                
	else 
	{
		HSV[1] = 0;
		HSV[0] = -1.0;
	}

	if(max==min)
	{	   HSV[0] = 0; 	

	}
	else
	{
		//-------------
		/*	 if( r == max )
		HSV[0] = ( g - b ) / delta;              // between yellow & magenta
		else if( g == max )
		HSV[0] = 2.0 + ( b - r ) / delta;        // between cyan & yellow
		else
		HSV[0] = 4.0 + ( r - g ) / delta;        // between magenta & cyan
		*/

		HSV[0] = ( r == max ) ? ( ( g - b ) / delta) : // between yellow & magenta
			( g == max ) ? (2.0 + ( b - r ) / delta) : // between cyan & yellow
			(4.0 + ( r - g ) / delta); // between magenta & cyan

	}

	HSV[0] = HSV[0] < 0 ? (HSV[0]*60)+360 : HSV[0]*60;// degrees

}

#if(OBJECTS_COLOR_TYPE_ONOFF)

int OTD_ObjectsColorType_Detected( OTD_Handel *otd_handle,
		const unsigned char* __restrict ImageR,const unsigned char* __restrict ImageG,const unsigned char* __restrict ImageB,
								  const int  width, const int  height, objects_points  *Objects, const int ObjectsAmount )
{
	int i,  temp , temp1 ,ii , jj , MaxColor;
	float HSV[3]; // HSV[0]= H ; HSV[1] = S ; HSV[2] = V ;
	ObjectsColorType ColorType;
	float ObjectsColorHistogramTotal,MaxObjectsColorHistogram;
	int SortObjectsColorTypeAmount = OBJECTSCOLORTYPE_THEBOTTOM-1;

	if(OBJECTSCOLORTYPE_THEBOTTOM < 1 )
		return 0;

	i = 0;
	while(i < ObjectsAmount)
	{
		memset( otd_handle->ObjectsColorHistogram , 0 , sizeof(float) * OBJECTSCOLORTYPE_THEBOTTOM);
		ObjectsColorHistogramTotal = 0;
		memset( Objects[i].Color , -1 , sizeof(objects_color_type_define) * OBJECTS_COLOR_TYPE_DEFINE_AMOUNT);

		for(ii = Objects[i].nLeftTopY ; ii<  Objects[i].nRightBottomY ; ++ii )
		{
			temp = ii * width;
			for(jj = Objects[i].nLeftTopX ; jj<  Objects[i].nRightBottomX ; ++jj )
			{
				temp1 = temp + jj;
				RGB2HSV( ImageR[temp1], ImageG[temp1], ImageB[temp1], HSV );

				if( HSV[1]<= 0.2 )
				{
					ColorType = ( HSV[2] >= 0.85 ) ? OBJECTSCOLORTYPE_WHITE :
						(HSV[2] >= 0.15 && HSV[2] < 0.85 ) ? OBJECTSCOLORTYPE_GRAY : OBJECTSCOLORTYPE_BLACK;
				}
				else if( HSV[1]> 0.2  &&  HSV[2] > 0.2  )
				{
					ColorType = ( HSV[0]< 30 || HSV[0] >= 330) ? 
					OBJECTSCOLORTYPE_RED : ( HSV[0] >= 30 && HSV[0]< 90) ?
					OBJECTSCOLORTYPE_YELLOW : ( HSV[0] >= 90 &&   HSV[0] < 150) ?
					OBJECTSCOLORTYPE_GREEN : ( HSV[0] >= 150 &&   HSV[0] < 210) ?
					OBJECTSCOLORTYPE_CYAN : ( HSV[0] >= 210 &&   HSV[0] < 270) ?
					OBJECTSCOLORTYPE_BLUE : ( HSV[0] >= 270 &&   HSV[0] < 330) ?
					OBJECTSCOLORTYPE_MAGENTA : OBJECTSCOLORTYPE_INVALID; // will NOT appear OBJECTSCOLORTYPE_INVALID
				}
				++otd_handle->ObjectsColorHistogram[ColorType];
			}
		}

		for(ii =0; ii<  OBJECTSCOLORTYPE_THEBOTTOM ; ++ii )
			ObjectsColorHistogramTotal += otd_handle->ObjectsColorHistogram[ii];
 
		if(ObjectsColorHistogramTotal > 0)
		{ 
		for(ii =0; ii<  OBJECTSCOLORTYPE_THEBOTTOM ; ++ii )
			otd_handle->ObjectsColorHistogram[ii] = (otd_handle->ObjectsColorHistogram[ii]/ObjectsColorHistogramTotal)*100.0;

		for(ii =0; ii<  OBJECTS_COLOR_TYPE_DEFINE_AMOUNT ; ++ii )
		{
			MaxColor = -1; MaxObjectsColorHistogram = 0;
			for(jj = 0 ; jj< OBJECTSCOLORTYPE_THEBOTTOM ; ++jj )
			{
				if(MaxObjectsColorHistogram <  otd_handle->ObjectsColorHistogram[jj] )
				{
					MaxColor = jj;
					MaxObjectsColorHistogram = otd_handle->ObjectsColorHistogram[jj];
				}
			}

			if(MaxColor >= 0)
			{
				Objects[i].Color[ii].ColorPercentage = (int)otd_handle->ObjectsColorHistogram[MaxColor];
				Objects[i].Color[ii].ColorType = (ObjectsColorType)MaxColor;
				otd_handle->ObjectsColorHistogram[MaxColor] = 0;
			}
			else
				break;
		}
		}

		++i;
	}

	return 1;

}

int OTD_ObjectsColorTypeWithForeground_Detected( OTD_Handel *otd_handle, const unsigned char *Foregroundimage,
		const unsigned char* ImageR,const unsigned char* ImageG,const unsigned char* ImageB,
		const int  width, const int  height, objects_points  *Objects, const int ObjectsAmount)
{

	int i,  temp , temp1 ,ii , jj , MaxColor;
	float HSV[3]; // HSV[0]= H ; HSV[1] = S ; HSV[2] = V ;
	ObjectsColorType ColorType;
	float ObjectsColorHistogramTotal,MaxObjectsColorHistogram;
	int SortObjectsColorTypeAmount = OBJECTSCOLORTYPE_THEBOTTOM-1;

	if(OBJECTSCOLORTYPE_THEBOTTOM < 1 )
		return 0;

	i = 0;
	while(i < ObjectsAmount)
	{
		memset( otd_handle->ObjectsColorHistogram , 0 , sizeof(float) * OBJECTSCOLORTYPE_THEBOTTOM);
		ObjectsColorHistogramTotal = 0;
		memset( Objects[i].Color , -1 , sizeof(objects_color_type_define) * OBJECTS_COLOR_TYPE_DEFINE_AMOUNT);

		for(ii = Objects[i].nLeftTopY ; ii<  Objects[i].nRightBottomY ; ++ii )
		{
			temp = ii * width;
			for(jj = Objects[i].nLeftTopX ; jj<  Objects[i].nRightBottomX ; ++jj )
			{
				temp1 = temp + jj;
				if( Foregroundimage[temp1] == FOREGROUND )
				{	
			     	RGB2HSV( ImageR[temp1], ImageG[temp1], ImageB[temp1], HSV );
					
					ColorType = OBJECTSCOLORTYPE_INVALID;
					
					if( HSV[1]<= 0.20 )
					{
						ColorType = ( HSV[2] >= 0.85 ) ? OBJECTSCOLORTYPE_WHITE :
							(HSV[2] >= 0.15 && HSV[2] < 0.85 ) ? OBJECTSCOLORTYPE_GRAY : OBJECTSCOLORTYPE_BLACK;
					}
					else if( HSV[1]> 0.20 &&  HSV[2] > 0.20 )
					{
						ColorType = ( HSV[0]< 30 || HSV[0] >= 330) ? 
							OBJECTSCOLORTYPE_RED : ( HSV[0] >= 30 && HSV[0]< 90) ?
							OBJECTSCOLORTYPE_YELLOW : ( HSV[0] >= 90 &&   HSV[0] < 150) ?
							OBJECTSCOLORTYPE_GREEN : ( HSV[0] >= 150 &&   HSV[0] < 210) ?
							OBJECTSCOLORTYPE_CYAN : ( HSV[0] >= 210 &&   HSV[0] < 270) ?
							OBJECTSCOLORTYPE_BLUE : ( HSV[0] >= 270 &&   HSV[0] < 330) ?
							OBJECTSCOLORTYPE_MAGENTA : OBJECTSCOLORTYPE_INVALID; // will NOT appear OBJECTSCOLORTYPE_INVALID
					}
					
					if(ColorType != OBJECTSCOLORTYPE_INVALID)
						++otd_handle->ObjectsColorHistogram[ColorType];
				}
			}
		}

		for(ii =0; ii<  OBJECTSCOLORTYPE_THEBOTTOM ; ++ii )
			ObjectsColorHistogramTotal += otd_handle->ObjectsColorHistogram[ii];
 
		if(ObjectsColorHistogramTotal > 0)
		{
			for(ii =0; ii<  OBJECTSCOLORTYPE_THEBOTTOM ; ++ii )
				otd_handle->ObjectsColorHistogram[ii] = (otd_handle->ObjectsColorHistogram[ii]/ObjectsColorHistogramTotal)*100.0;
 
			for(ii =0; ii<  OBJECTS_COLOR_TYPE_DEFINE_AMOUNT ; ++ii )
			{
				MaxColor = -1; MaxObjectsColorHistogram = 0;
				for(jj = 0 ; jj< OBJECTSCOLORTYPE_THEBOTTOM ; ++jj )
				{
					if(MaxObjectsColorHistogram <  otd_handle->ObjectsColorHistogram[jj] )
					{
						MaxColor = jj;
						MaxObjectsColorHistogram = otd_handle->ObjectsColorHistogram[jj];
					}
				}

				if(MaxColor >= 0)
				{
					Objects[i].Color[ii].ColorPercentage = (int)otd_handle->ObjectsColorHistogram[MaxColor];
					Objects[i].Color[ii].ColorType = (ObjectsColorType)MaxColor;
					otd_handle->ObjectsColorHistogram[MaxColor] = 0;
				}
				else
					break;
			}

		}
 
		++i;

	}
 
	return 1;
}

#endif
