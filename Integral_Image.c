/*
 * Integral_Image.c
 *
 *  Created on:  
 *      Author: Jeff Yang
 */

#include "Integral_Image.h"

//------------------------------------------------------------------------------------
int IntegralImage_Difference_Feature(int* integralImage, int integralImageWidth, int integralImageHeight , int x,int y, int xRange, int yRange , int choiceFeature)
{

	int subXrange;
	int subYrange;
	int recsize = 0;
	int averageReturn = 0;
 	int temp,temp2;
	//---------
		int sum = 0,sum1 = 0 ;
	//---------

	switch(choiceFeature)
	{

		case 1:
		//回傳A的一個feature中的Gray Difference

			subXrange =  xRange >> 1;
			temp = y*integralImageWidth + x;
 			 sum =   integralImage[temp] -  (integralImage[temp+subXrange]<<1) + integralImage[temp+xRange];
			temp = temp + yRange*integralImageWidth;
			recsize =   subXrange*yRange;
			 sum = sum - integralImage[temp] + (integralImage[temp+subXrange]<<1) - integralImage[temp+xRange];
 			break;

		case 2:

			subYrange =  yRange >> 1;
			temp = y*integralImageWidth + x;
			 	sum =   integralImage[ temp ] - integralImage[ temp+xRange ];
			temp2 = temp +  subYrange*integralImageWidth;
			 	sum = sum - ( (integralImage[temp2] - integralImage[ temp2+xRange ])<<1 );
			temp2 = temp + yRange*integralImageWidth;
			recsize =   xRange*subYrange ;
				 sum  = sum + integralImage[temp2] - integralImage[ temp2+xRange ];
 			break;

		case 3:

			subXrange =  (int)( (float)(xRange)/3.0);
			temp = y*integralImageWidth + x;
			temp2 = subXrange + subXrange ;
		 	sum = integralImage[ temp] - integralImage[ temp+ subXrange ] + integralImage[ temp+temp2 ]- integralImage[ temp+xRange ];
		 	sum1 = integralImage[ temp+ subXrange ]-integralImage[ temp+ temp2 ];
			temp = temp + yRange*integralImageWidth;
			 sum = sum - integralImage[temp] + integralImage[ temp+ subXrange ] - integralImage[ temp+ temp2 ]+integralImage[ temp+ xRange ];
			 sum /=2;
			 sum1  = sum1 + integralImage[ temp+ temp2 ]-integralImage[ temp+ subXrange ];
			 sum  = sum - sum1;
			recsize = yRange*subXrange ;

			break;

		case 4:

			subXrange =  xRange >> 1;
			subYrange =  yRange >> 1;

			temp = y*integralImageWidth + x;
			 sum =  integralImage[temp] - 2*integralImage[ temp+subXrange ] + integralImage[ temp+xRange ];
			temp2 = temp + subYrange*integralImageWidth;
			 sum = sum - 2*integralImage[ temp2 ] + 4*integralImage[ temp2+subXrange ] - 2*integralImage[ temp2+xRange ];
			 temp2 = temp + yRange*integralImageWidth;
			 sum  = sum + integralImage[temp2] - 2*integralImage[ temp2+subXrange ] + integralImage[ temp2+xRange ];
			 sum1 = sum;
			 sum /= 2;
 			recsize = subXrange*subYrange ;
			break;

		case 5:

			subYrange = yRange/6;
			temp = y*integralImageWidth + x;
			sum =  integralImage[ temp ] - integralImage[ temp+ xRange ];
			temp2 = temp + subYrange*integralImageWidth;
			sum = sum + integralImage[ temp2 + xRange ] - integralImage[ temp2 ];
			sum1 = integralImage[ temp2 ] - integralImage[ temp2 + xRange ];
			temp2 = temp2 + 4*subYrange*integralImageWidth;
			sum  = sum + integralImage[temp2] - integralImage[ temp2+ xRange ] ;
			sum1  = sum1 + integralImage[ temp2+ xRange ] - integralImage[temp2];
			temp2 = temp + yRange*integralImageWidth;
			sum  = sum + integralImage[ temp2+xRange ] - integralImage[temp2] ;
			sum /= 2;
			sum1 /= 4;
			sum = sum1 - sum;
			recsize = xRange*subYrange ;
			break;

		default:
			printf("Difference_Feature() error! \n");
			printf("Average() error! \n");
	}

	averageReturn =	round( abs(sum) / (float)(recsize) );

	return averageReturn;
}
 
void IntegralImage_IntegralImage_Gray( unsigned char *image_s, const int width  , const int height , int *image_d)
{
	int i,j,temp;
	int rowsum = 0;

	temp = width*height;

	// first row
	for(j=0; j<width; j++)
	{
 		rowsum += image_s[j];
		image_d[j] = rowsum;
	}

	// remaining cells are sum above and to the left
	for(i=1; i<height; ++i)
	{
		rowsum = 0;
		for( j=0; j<width; ++j)
		{
			temp = i*width+j;
			rowsum += image_s[temp];
			image_d[temp] = rowsum + image_d[temp - width];
		}
	}

}

int IntegralImage_Get_Feature_Position(int* info , int position, int xSize ,int ySize , int LearnerIndex , int* FeatureCounterEnd)
{  //input 第幾個feature
	//return 第幾種feature,起始位置,subFeature大小
	//FeatureInfo[5]={feature type(1~4),xPosition,yPosition,xRange,yRange}

	int xPosition; //目前feature的位置
	int yPosition;
	int xRange; //目前feature的大小
	int yRange;
	int counter = 0;
 
	//feature A
    if(FeatureCounterEnd[0] == -1 || ( 0 <= position &&  position < FeatureCounterEnd[0] ) )
    {
	    for (xRange = 2; xRange <= xSize; xRange += 2)
	    {
		    for (yRange = 1; yRange <= ySize; yRange++)
		    {
		    	for (yPosition = 0; yPosition <= (ySize - yRange); yPosition++)
			    {
				    for (xPosition = 0; xPosition <= (xSize - xRange); xPosition++)
				    {
					    if(position!=counter)
					    	counter++;
					    else
					    {
 						    info[0] = 1;
						    info[1] = xPosition; info[2] = yPosition;
						    info[3] = xRange; info[4] = yRange;

						    return 1;
					    }
				    }
			    }
		    }
	    }

	FeatureCounterEnd[0] = counter;
 
    }

    if(FeatureCounterEnd[0] != -1 && counter == 0)
	    counter  = FeatureCounterEnd[0];

	//feature B
    if(FeatureCounterEnd[1] == -1 || ( FeatureCounterEnd[0] <= position &&  position < FeatureCounterEnd[1] ) )
    {
	    for (xRange = 1; xRange <= xSize; xRange++)
	    {
		    for (yRange = 2; yRange <= ySize; yRange += 2)
		    {
			    for (yPosition = 0; yPosition <= (ySize - yRange); yPosition++)
			    {
                    for (xPosition = 0; xPosition <= (xSize - xRange); xPosition++)
				    {
					    if(position!=counter)
						    counter++;
					    else
					    {
 					    	info[0] = 2;
						    info[1] = xPosition; info[2] = yPosition;
						    info[3] = xRange; info[4] = yRange;

						    return 1;
					    }
				    }
			    }
		    }
	    }
 	FeatureCounterEnd[1] = counter;
 
    }

    if(FeatureCounterEnd[1] != -1  && counter == FeatureCounterEnd[0])
	    counter  = FeatureCounterEnd[1];

    //feature C
    if(FeatureCounterEnd[2] == -1 || ( FeatureCounterEnd[1] <= position &&  position < FeatureCounterEnd[2] ) )
    {
	    for (xRange = 3; xRange <= xSize; xRange += 3)
	    {
		    for (yRange = 1; yRange <= ySize; yRange++)
		    {
			    for (yPosition = 0; yPosition <= (ySize - yRange); yPosition++)
			    {
				    for (xPosition = 0; xPosition <= (xSize - xRange); xPosition++)
				    {
					    if(position!=counter)
					     	counter++;
					    else
					    {
 					    	info[0] = 3;
					    	info[1] = xPosition; info[2] = yPosition;
						    info[3] = xRange; info[4] = yRange;

						    return 1;
					    }
				    }
			    }
		    }
	    }
 	FeatureCounterEnd[2] = counter;
 
    }

    if(FeatureCounterEnd[2] != -1  && counter ==  FeatureCounterEnd[1] )
	    counter  = FeatureCounterEnd[2];

    //feature D
    if(FeatureCounterEnd[3] == -1 || ( FeatureCounterEnd[2] <= position &&  position < FeatureCounterEnd[3] ) )
    {
	    for (xRange = 2; xRange <= xSize; xRange += 2)
	    {
		    for (yRange = 2; yRange <= ySize; yRange += 2)
		    {
			    for (yPosition = 0; yPosition <= (ySize - yRange); yPosition++)
			    {
				    for (xPosition = 0; xPosition <= (xSize - xRange); xPosition++)
				    {
					    if(position!=counter)
					    	counter++;
					    else
					    {
 						    info[0] = 4;
						    info[1] = xPosition; info[2] = yPosition;
						    info[3] = xRange; info[4] = yRange;
					    	return 1;
					    }
				    }
			    }
		    }
	    }

 	FeatureCounterEnd[3] = counter;
 
    }

    if(FeatureCounterEnd[3] != -1  && counter == FeatureCounterEnd[2] )
	    counter  = FeatureCounterEnd[3];

    //feature E
    if(FeatureCounterEnd[4] == -1 || ( FeatureCounterEnd[3] <= position &&  position < FeatureCounterEnd[4] ) )
    {
	    for (xRange = 1; xRange <= xSize; xRange++)
	    {
		    for (yRange = 6; yRange <= ySize; yRange += 6)
		    {
			    for (yPosition = 0; yPosition <= (ySize - yRange); yPosition++)
			    {
			    	for (xPosition = 0; xPosition <= (xSize - xRange); xPosition++)
				    {
				    	if(position!=counter)
					    	counter++;
					    else
					    { 
						    info[0] = 5;
						    info[1] = xPosition; info[2] = yPosition;
						    info[3] = xRange; info[4] = yRange;
						    return 1;
					    }
				    }
			    }    
		    }
	    }

 	FeatureCounterEnd[4] = counter;
 
    }

    if(FeatureCounterEnd[4] != -1  && counter ==  FeatureCounterEnd[3] )
	    counter  = FeatureCounterEnd[4];
 	    info[0] = info[1] = info[2] = info[3] = info[4] = -1;
	    return 0;

} //end of getFeaturePosition() 

void ImageScale_ScaleLineAvg(unsigned char *Target, unsigned char *Source, int SrcWidth, int TgtWidth)
{
	int NumPixels = TgtWidth;
	int Mid = TgtWidth / 2;
	int E = 0;
	unsigned char p;

	if (TgtWidth > SrcWidth)
		NumPixels--;
	while (NumPixels-- > 0)
	{
		p = *Source;
		if (E >= Mid)
			p = ImageScale_average(p, *(Source+1));
		*Target++ = p;
		E += SrcWidth;
		if (E >= TgtWidth)
		{
			E -= TgtWidth;
			Source++;
		}
	}
	if (TgtWidth > SrcWidth)
		*Target = *Source;
}

void ImageScale_ScaleLine(unsigned char *Target, unsigned char *Source, int SrcWidth, int TgtWidth)
{
	int NumPixels = TgtWidth;
	int IntPart = SrcWidth / TgtWidth;
	int FractPart = SrcWidth % TgtWidth;


	int E = 0;
	while (NumPixels-- > 0)
	{
		*Target++ = *Source;
		Source += IntPart;
 
		//只有 SrcWidth < TgtWidth 成立
		E += FractPart;

		if (E >= TgtWidth)
		{
			E -= TgtWidth;
			Source++;

		}
	}
}

void ImageScale_ScaleRect( unsigned char *Source, int SrcWidth, int SrcHeight, unsigned char *Target,  int TgtWidth, int TgtHeight)
{
	int NumPixels = TgtHeight;
	int IntPart = (SrcHeight / TgtHeight) * SrcWidth;
	int FractPart = SrcHeight % TgtHeight;
	int E = 0;
	unsigned char *PrevSource = 0;

	while (NumPixels-- > 0)
	{
		if (Source == PrevSource)
		{
			memcpy(Target, Target-TgtWidth, TgtWidth*sizeof(*Target));
		}
		else
		{
			ImageScale_ScaleLine(Target, Source, SrcWidth, TgtWidth);
 			PrevSource = Source;
		} /* if */

		Target += TgtWidth;
		Source += IntPart;
		E += FractPart;

		if (E >= TgtHeight)
		{
			E -= TgtHeight;
			Source += SrcWidth;
		} /* if */
	} /* while */
}

