/*
   ============================================================================
Name        : Virtual_Fence.c
Author      : Jeff Yang
Version     :
Virtual Fence created on: 2010/5/14
Directional Detection created on: 2010/5/24
Virtual_Fence_Change_Resolution modified on: 2010/10/15 Add no destroy by checking width and height if the same with the prior setting.
 
Modify : 2010/12/23:  Remove DIRECTIONAL_DETECTION 

Copyright   : Jeff Yang
Description : Virtual Fence in C, Ansi-style
============================================================================
*/


#include "Virtual_Fence.h"


/* ========================================================================
   Point_In_Poly

Input:
point *pPoly  			�h����I x y
const int nPoints		�h����I�ƶq
const unsigned int unXt x�I�y��
const unsigned int unYt y�I�y��
Output:
none
Return :
1 inside
0 outside
Description :
is target point inside a 2D polygon?
polygon points, [0]=x, [1]=y
number of points in polygon
x (horizontal) of target point
y (vertical) of target point
======================================================================== */
short Point_In_Poly( point *pPoly , const int nPoints, const unsigned int unXt, const unsigned int unYt)
{
	unsigned int xnew,ynew;
	unsigned int xold,yold;
	unsigned int x1,y1;
	unsigned int x2,y2;
	int i;
	short sInside=0;

	if (nPoints < 3)
		return(0);


	xold = pPoly[nPoints-1].x;//[0];
	yold = pPoly[nPoints-1].y;//[1];

	for (i=0 ; i < nPoints ; i++)
	{
		xnew = pPoly[i].x;//[0];
		ynew = pPoly[i].y;//[1];

		if (xnew > xold)
		{
			x1=xold;
			x2=xnew;
			y1=yold;
			y2=ynew;
		}
		else
		{
			x1=xnew;
			x2=xold;
			y1=ynew;
			y2=yold;
		}

		if ( (xnew < unXt) == (unXt <= xold)         /* edge "open" at left end */
				&& ((long)unYt-(long)y1)*(long)(x2-x1)
				< ((long)y2-(long)y1)*(long)(unXt-x1) )
		{
			sInside=!sInside;
		}

		xold = xnew;
		yold = ynew;
	}

	return(sInside);
}

/* ========================================================================
   Virtual_Fence_Initial

Input:
HandleVF *Handle
Output:
none
Return :
1	Created successfully
Description :
Create pImageMap and assign 0
======================================================================== */
int Virtual_Fence_Initial( HandleVF *Handle )
{

	Handle->pImageMap  = (unsigned char*)  malloc(  Handle->nWidth * Handle->nHeight );
	memset(Handle->pImageMap , 0 ,  Handle->nWidth * Handle->nHeight );

	return 1;
}

/* ========================================================================
   Virtual_Fence_Destroy

Input:
HandleVF *Handle
Output:
none
Return :
1	Created successfully

Description :
Destroy pImageMap
======================================================================== */
int Virtual_Fence_Destroy( HandleVF *Handle)
{
	free(Handle->pImageMap);
	Handle->pImageMap = 0;

	free(Handle );
	Handle = 0;

#ifdef FOXINT_PRINTF
	printf("%s %d\n", __FUNCTION__ , __LINE__);
#endif

	return 1;
}
/* ========================================================================
   Virtual_Fence_Created

Input:
HandleVF *Handle
unsigned int nWidth;   �e�� �e /
unsigned int nHeight;  �e�� ��
polygon *Polygons			Polygons's struct
const int nPolygonsAmount	Polygons's �ƶq
Output:
none
Return :
0	Created successfully
-1	Created fail
Description :
��l�� pImageMap �è̾� Polygons �إ� Image Map
�h��ΰϰ�s��1~8 => 00000001,00000010,00000100,...,10000000
======================================================================== */
int Virtual_Fence_Created( HandleVF **Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons, const int nPolygonsAmount)
{
	int i,j,temp,x,y;
	unsigned int pMaxXY[2] , pMinXY[2];
	unsigned char ucShiftTemp;
	//Added, Bruce Hsu, 06/11/2010
	HandleVF *HandleV;
	//Added, Bruce Hsu, 06/11/2010
	*Handle = HandleV = (HandleVF *) malloc( sizeof(HandleVF) );
	HandleV->nWidth = nWidth;
	HandleV->nHeight = nHeight;

	if(nPolygonsAmount <= 0 || nPolygonsAmount > MAX_POLYGON_AMOUNT)
		return -1;

	//---Modified,Jeff Yang,2010/09/06
	//*Handle = (HandleVF *)_cache_malloc(sizeof(HandleVF),-1);
	//(*Handle)->nHeight = nWidth;
	//(*Handle)->nWidth = nHeight;
	//unsigned char pImageMapValue[OBJECT_NUMBER];
	HandleV->nPolygonsAmount = nPolygonsAmount;

	Virtual_Fence_Initial( HandleV );

	//---

	//--------------------------------------------------
	// �h��μƥ�
	for(x = 0; x < nPolygonsAmount; ++x)
	{
		pMaxXY[0] =	pMinXY[0] =  Polygons[x].Points[0].x; //x
		pMaxXY[1] =	pMinXY[1] =  Polygons[x].Points[0].y; //y

		// �h��Ψ��I�� �M��̤j�̤p x y
		for(y = 1; y <  Polygons[x].nPointsNumber; ++y)
		{
			pMaxXY[0] = pMaxXY[0] < Polygons[x].Points[y].x  ? Polygons[x].Points[y].x : pMaxXY[0];
			pMaxXY[1] = pMaxXY[1] < Polygons[x].Points[y].y  ? Polygons[x].Points[y].y : pMaxXY[1];

			pMinXY[0] = pMinXY[0] > Polygons[x].Points[y].x  ? Polygons[x].Points[y].x : pMinXY[0];
			pMinXY[1] = pMinXY[1] > Polygons[x].Points[y].y  ? Polygons[x].Points[y].y : pMinXY[1];
		}

		// �h��� 1,2,3,...,8
		ucShiftTemp = 0x01 << x;
		//--------------------------------------------------
		//nHeight
		for(i = pMinXY[1] ; i< pMaxXY[1]; ++i)
		{			temp = i * HandleV->nWidth;
			//nWidth
			for(j =  pMinXY[0] ; j< pMaxXY[0];  ++j)
			{
				if( Point_In_Poly( Polygons[x].Points , Polygons[x].nPointsNumber , j , i ) )
					HandleV->pImageMap[temp+j] |= ucShiftTemp;
			}
		}
		//--------------------------------------------------
	}

	return 0;
}


/* ========================================================================
   Virtual_Fence_Running

Input:
HandleVF *Handle
objects AllObjects			����(�]�t���󥪤Wx,y�M�k�Ux,y and ����ƶq �̦hOBJECT_NUMBER��)
Output:
unsigned char* pImageMapValue	�����л\ ���ϰ�s��(�̦hOBJECT_NUMBER��)
EX:
pImageMapValue[2] = 00000011 means the object cover no.1 and no.2 polygon
Return :
none
Description :
��J�إߦn������struct AllObjects �N��X����ҹ��� ���ϰ�s���}�C pImageMapValue
======================================================================== */
void Virtual_Fence_Running( HandleVF *Handle ,  objects  AllObjects , unsigned char* pImageMapValue)
{
	int i, temp, temp1 ,temp0, j;

	for( j=0; j < AllObjects.usObjectsAmount; ++j )
	{
		pImageMapValue[j] = 0;
		//----------------------------------------
		// �|��
		// �W �T�w���Wy
		// �U �T�w�k�Uy
		i = AllObjects.ObjectsNumber[j].nLeftTopX;
		temp = AllObjects.ObjectsNumber[j].nLeftTopY * Handle->nWidth;
		temp1 = AllObjects.ObjectsNumber[j].nRightBottomY * Handle->nWidth;
		while( AllObjects.ObjectsNumber[j].nRightBottomX >= i )
		{
			pImageMapValue[j] |= Handle->pImageMap[ temp+i ] | Handle->pImageMap[ temp1+i  ];
			i += SHIFT_PIXEL;
		}

		// �� �T�w���Wx
		// �k �T�w�k�Ux
		i = AllObjects.ObjectsNumber[j].nLeftTopY;
		temp0 = AllObjects.ObjectsNumber[j].nLeftTopY  * Handle->nWidth;
		temp =  temp0 + AllObjects.ObjectsNumber[j].nLeftTopX ;
		temp1 =  temp0 + AllObjects.ObjectsNumber[j].nRightBottomX ;
		while( AllObjects.ObjectsNumber[j].nRightBottomY >= i )
		{
			pImageMapValue[j] |= Handle->pImageMap[ temp ] | Handle->pImageMap[ temp1 ]; //CYCLES->[ 3015 ]
			temp0 =  Handle->nWidth * SHIFT_PIXEL ;
			temp += temp0;
			temp1 += temp0;
			i += SHIFT_PIXEL;
		}
	}
}


/* ========================================================================
   Virtual_Fence_Change_Resolution

Input:
HandleVF *Handle
polygon *Polygons			Polygons's struct
const int nPolygonsAmount	Polygons's �ƶq
Output:
none
Return :
0	Created successfully
-1	Failed
Description :
�� Polygons's ��m���� �ݭn�I�s���禡 ���s�إ� Image Map
======================================================================== */
int Virtual_Fence_Change_Resolution( HandleVF *Handle, unsigned int nWidth, unsigned int nHeight, polygon *Polygons, const int nPolygonsAmount)
{
	int i,j,temp,x,y;
	unsigned int pMaxXY[2] , pMinXY[2];
	unsigned char ucShiftTemp;

	//Modify,Jeff Yang,2010/10/15
	if(nPolygonsAmount <= 0 || nPolygonsAmount > MAX_POLYGON_AMOUNT)
		return -1;

	if(nWidth != Handle->nWidth || nHeight != Handle->nHeight)
	{
		Virtual_Fence_Destroy( ( Handle) );
		Virtual_Fence_Created(  &Handle , nWidth, nHeight, Polygons , nPolygonsAmount );
	}
	else
	{
		Handle->nPolygonsAmount = nPolygonsAmount;
		memset(Handle->pImageMap , 0 ,  Handle->nWidth * Handle->nHeight );
		//--------------------------------------------------
		// �h��μƥ�
		for(x = 0; x < nPolygonsAmount; ++x)
		{
			pMaxXY[0] =	pMinXY[0] =  Polygons[x].Points[0].x; //x
			pMaxXY[1] =	pMinXY[1] =  Polygons[x].Points[0].y; //y

			// �h��Ψ��I�� �M��̤j�̤p x y
			for(y = 1; y <  Polygons[x].nPointsNumber; ++y)
			{
				pMaxXY[0] = pMaxXY[0] < Polygons[x].Points[y].x  ? Polygons[x].Points[y].x : pMaxXY[0];
				pMaxXY[1] = pMaxXY[1] < Polygons[x].Points[y].y  ? Polygons[x].Points[y].y : pMaxXY[1];
				pMinXY[0] = pMinXY[0] > Polygons[x].Points[y].x  ? Polygons[x].Points[y].x : pMinXY[0];
				pMinXY[1] = pMinXY[1] > Polygons[x].Points[y].y  ? Polygons[x].Points[y].y : pMinXY[1];
			}
			//	printf("[%d,%d,%d,%d]\n", pMaxXY[0], pMaxXY[1], pMinXY[0] , pMinXY[1]);
			// �h��� 1,2,3,...,8
			ucShiftTemp = 0x01 << x;
			//--------------------------------------------------
			//nHeight
			for(i = pMinXY[1] ; i< pMaxXY[1]; ++i)
			{			temp = i * Handle->nWidth;
				//nWidth
				for(j =  pMinXY[0] ; j< pMaxXY[0];  ++j)
				{
					if( Point_In_Poly( Polygons[x].Points , Polygons[x].nPointsNumber , j , i ) )
						Handle->pImageMap[temp+j] |= ucShiftTemp;
				}
			}
			//--------------------------------------------------
		}
		//--------------------------------------------------

	}
	return 0;
}


