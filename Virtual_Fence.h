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
	unsigned char *pImageMap; /* �h��ΰϰ� Image Map	�j�p���ثe�e�����e  */
	unsigned int nWidth;  /*�e�� �e*/
	unsigned int nHeight; /*�e�� ��*/
 	int nPolygonsAmount;
 
} HandleVF;

//---------------------------------------------

 
//---------------------------------------------
/* �ǤJHandleVF *Handle �{�������ݩI�sDestroy  unsigned char *pImageMap */
int Virtual_Fence_Destroy( HandleVF *Handle );

/* �ǤJHandleVF *Handle, Polygons �h��ε��cx,y,nPointsNumber , nPolygonsAmount �h��μƶq */
//int Virtual_Fence_Created( HandleVF *Handle , polygon *Polygons , const int nPolygonsAmount);
//Modified,Jeff Yang,2010/09/06
int Virtual_Fence_Created( HandleVF **Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons, const int nPolygonsAmount);

/* �ǤJHandleVF *Handle , AllObjects ���󵲺cnLeftTopX nLeftTopY nRightBottomX nRightBottomY unObjectID usObjectsAmount
   , pImageMapValue ��X ����ҹ����h��νs�� */
void Virtual_Fence_Running( HandleVF *Handle ,  objects  AllObjects , unsigned char *pImageMapValue);

//Modified,Jeff Yang,2010/09/08
/*The same with Virtual_Fence_Created(...) */
//int Virtual_Fence_Change_Resolution( HandleVF **Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons , const int nPolygonsAmount);
int Virtual_Fence_Change_Resolution( HandleVF  *Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons , const int nPolygonsAmount);

//---------------------------------------------


#endif /* VIRTUAL_FENCE_H_ */
