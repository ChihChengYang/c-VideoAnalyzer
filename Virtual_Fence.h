/*
 * Virtual_Fence.h
 *
 *  		Virtual Fence created on: 2010/5/14
 *      Author: Jeff Yang  
 */

#ifndef VIRTUAL_FENCE_H_
#define VIRTUAL_FENCE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
 
#include "IVAI_Parameters.h"
 
#define SHIFT_PIXEL  1

  
typedef struct HandleVF
{
	unsigned char *pImageMap; /* 多邊形區域 Image Map	大小為目前畫面長寬  */
	unsigned int nWidth;  /*畫面 寬*/
	unsigned int nHeight; /*畫面 長*/
 	int nPolygonsAmount;
 
} HandleVF;

//---------------------------------------------

 
//---------------------------------------------
/* 傳入HandleVF *Handle 程式結束需呼叫Destroy  unsigned char *pImageMap */
int Virtual_Fence_Destroy( HandleVF *Handle );

/* 傳入HandleVF *Handle, Polygons 多邊形結構x,y,nPointsNumber , nPolygonsAmount 多邊形數量 */
//int Virtual_Fence_Created( HandleVF *Handle , polygon *Polygons , const int nPolygonsAmount);
//Modified,Jeff Yang,2010/09/06
int Virtual_Fence_Created( HandleVF **Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons, const int nPolygonsAmount);

/* 傳入HandleVF *Handle , AllObjects 物件結構nLeftTopX nLeftTopY nRightBottomX nRightBottomY unObjectID usObjectsAmount
   , pImageMapValue 輸出 物件所對應多邊形編號 */
void Virtual_Fence_Running( HandleVF *Handle ,  objects  AllObjects , unsigned char *pImageMapValue);

//Modified,Jeff Yang,2010/09/08
/*The same with Virtual_Fence_Created(...) */
//int Virtual_Fence_Change_Resolution( HandleVF **Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons , const int nPolygonsAmount);
int Virtual_Fence_Change_Resolution( HandleVF  *Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons , const int nPolygonsAmount);

//---------------------------------------------


#endif /* VIRTUAL_FENCE_H_ */
