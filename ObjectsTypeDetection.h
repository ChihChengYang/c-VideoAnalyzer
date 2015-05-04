/*
 * ObjectsTypeDetection.h
 *
 */

#ifndef OBJECTSTYPEDETECTION_H_
#define OBJECTSTYPEDETECTION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "IVAI_Parameters.h"

#define ADJUST_UNKNOWN_SIZE_BTWEEN_TWO_TYPE 0 //1: ON 0:OFF

#define OBJECTS_COLOR_TYPE_ONOFF 1 //1: ON 0:OFF

#define WIDTH_HEIGHT_RATIO_LOWER 0.7  // WIDTH_HEIGHT_RATIO_LOWER < 1

#define WIDTH_HEIGHT_RATIO_UPPER 1.4 // WIDTH_HEIGHT_RATIO_UPPER > 1

// maximum value of a,b,c
#define MAX3(a,b,c)  (((a)>(b))?(((a)>(c))?(a):(c)):(((b)>(c))?(b):(c)))
// minimum value of a,b,c
#define MIN3(a,b,c)  (((a)<(b))?(((a)<(c))?(a):(c)):(((b)<(c))?(b):(c)))
//sqrt
#define SQR(a)   ((a)*(a))

// for Sequential_KMeans
#define SEQUENTIAL_KMEANS_ONOFF 1 //1: ON 0:OFF

#if(SEQUENTIAL_KMEANS_ONOFF)

#define SEQUENTIAL_KMEANS_TYPE 1 //1: Distance 0 :

#define SEQUENTIAL_KMEANS_STORE 20 //

typedef struct SequentialKMeansObjectsStore
{
	int Width;
	int Height;
	int DistanceBetweenMean;

} SequentialKMeansObjectsStore;

typedef struct  SequentialKMeansObjects
{
	int MeanWidth;
	int MeanHeight;
	SequentialKMeansObjectsStore ObjectsStore[SEQUENTIAL_KMEANS_STORE];
	int ObjectsStoreCount;

} SequentialKMeansObjects;

#endif

typedef struct ObjectsSizeDefine
{
#if( !SEQUENTIAL_KMEANS_ONOFF || ( SEQUENTIAL_KMEANS_ONOFF && SEQUENTIAL_KMEANS_TYPE==0) )
	unsigned int lowerWidth;
	unsigned int lowerHeight;
	unsigned int upperWidth;
	unsigned int upperHeight;
	int lowerArea;
	int upperArea;
#endif

	ObjectsSizeType OST;

} ObjectsSizeDefine;

typedef struct OTD_Handel
{
	ObjectsSizeDefine *OSD;
	int objectsSizeDefineCount;

	float *ObjectsColorHistogram; // Size = OBJECTSCOLORTYPE_THEBOTTOM

#if(SEQUENTIAL_KMEANS_ONOFF)
	SequentialKMeansObjects *SKMObjects;
#endif

}OTD_Handel;

int ObjectsTypeDetection_Create( OTD_Handel **otd_handle, OTD_Parameters *OTD_Param );

int ObjectsTypeDetection_Destroy( OTD_Handel *otd_handle );

int ObjectsTypeDetection_Running( OTD_Handel *otd_handle, objects_points *Objects, dd_group *DD_Group, const int ObjectsAmount, IpcEventDataIntelligent_TypeData *pEvent );

int ObjectsTypeDetection_Parameters( OTD_Handel *otd_handle, OTD_Parameters *OTDParameters );

#if(OBJECTS_COLOR_TYPE_ONOFF)
int OTD_ObjectsColorType_Detected( OTD_Handel *otd_handle,const unsigned char* __restrict ImageR,const unsigned char* __restrict ImageG,const unsigned char* __restrict ImageB,
								  const int  width, const int  height, objects_points  *Objects, const int ObjectsAmount );

int OTD_ObjectsColorTypeWithForeground_Detected( OTD_Handel *otd_handle, const unsigned char *Foregroundimage,
		const unsigned char* ImageR, const unsigned char* ImageG,const unsigned char* ImageB,
		const int  width, const int  height, objects_points  *Objects, const int ObjectsAmount);
#endif


#endif /* OBJECTSTYPEDETECTION_H_ */
