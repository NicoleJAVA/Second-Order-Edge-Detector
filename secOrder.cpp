
#include "stdio.h"
#include "iostream.h"
#include "stdlib.h"
#include "math.h"

#define PI 3.14159265
#define Size 512
#define ThreshUp 33
#define ThreshLow 15
using namespace std;

int main( int argc, char *argv[])
{
	FILE *file;
	unsigned char Imagedata[Size][Size];
	
						/************************************************/
						/******                                	 ******/
						/****** Read "input.raw" into Imagedata[][]******/
						/******                                	 ******/
						/************************************************/							
	if (!(file=fopen("sample1.raw","rb"))){
		cout<<"Cannot open file!"<<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size*Size, file);
	fclose(file);

	/**********************************	do some image processing task... ***/
	
	float gauss[5][5] = {{1,4,7,4,1}, {4,16,26,16,4}, {7,26,41,26,7}, {4,16,26,16,4}, {1,4,7,4,1}};
	int i = 0, j = 0;
	int iOffset = 0, jOffset = 0; 	//	 the offset within the moving-window
	unsigned char afterGauss[Size][Size];
	float  neighborSum = 0;
		
	
						/********************************************/
						/******                                ******/
						/******     Blur with Gaussian Filter  ******/
						/******                                ******/
						/********************************************/	
	
	//								   < Construct Gaussian mask >
	
	for( i = 0; i < 5; i++){
		for( j = 0; j < 5; j++ ){
			gauss[i][j] = gauss[i][j] / 273.0;
		}
	}
	
	//								   < Move the Gaussian window >		
	
	for( i = 2; i < Size-2; i++ ){
		for( j = 2; j < Size-2; j++){
		
			neighborSum = 0;

			for( iOffset = -2; iOffset <= 2; iOffset++ ){
				for( jOffset = -2; jOffset <= 2; jOffset++ ){
					
					neighborSum +=  Imagedata[i+iOffset][j+jOffset] * gauss[2+iOffset][2+jOffset];


				}	//	End-4-for
			}	//	End-3-for					

			afterGauss[i][j] = static_cast<unsigned char>(neighborSum);
					
		}	//	End-2-for
	}	//	End-1-for





	
	
	
	
	
						/********************************************/
						/******                                ******/
						/******    Second Order ( i.e. Laplacian ! )      
						/******                                ******/
						/********************************************/
	
	float LaplacianMask[3][3] = {  {0,0.25,0}, {0.25,-1,0.25}, {0,0.25,0}  };						
	unsigned char afterSecOrder[Size][Size];
	
	
	//								   < Move the 2nd Dirivitive window >		
	
	for( i = 2; i < Size-2; i++ ){
		for( j = 2; j < Size-2; j++){
		
			neighborSum = 0;

			for( iOffset = -1; iOffset <= 1; iOffset++ ){
				for( jOffset = -1; jOffset <= 1; jOffset++ ){					
					neighborSum +=  Imagedata[i+iOffset][j+jOffset] * LaplacianMask[1+iOffset][1+jOffset];
				}	//	End-4-for
			}	//	End-3-for					
			afterSecOrder[i][j] = static_cast<unsigned char>(neighborSum);
		}	//	End-2-for
	}	//	End-1-for
	
	
	


	
						/**********************************************************/
						/******                                			***********/
						/******    Thresholding to Separate Zero and Non-zero
						/******    But this step does not decide 
						/******    whether it's zero-crossing. will do in next step       
						/******                                			***********/
						/**********************************************************/		
		
	static unsigned char afterThresh[Size][Size];
		//"static" avoids arr-too-big bug
	for( i = 2; i < Size - 2; i++ ){
		for( j = 2; j < Size - 2; j ++ ){
			
			if( afterSecOrder[i][j] > ThreshUp ){
				afterThresh[i][j] = 255;	
			}	
			else if( afterSecOrder[i][j]>=ThreshLow && afterSecOrder[i][j]<=ThreshUp ){
				afterThresh[i][j] = 127;
			}
			else if( afterSecOrder[i][j] < ThreshLow ){
				afterThresh[i][j] = 0;
			}
			
		}	//	End for
	}	//		End-1-for	



						/********************************************/
						/******                                ******/
						/******   Whether afterThresh[i][j] 
						/******   is zero-crossing	or not.	
						/******                                ******/
						/********************************************/	

	static unsigned char edgeMap[Size][Size];//"static" avoids arr-too-big bug
	int flag = 0;

	
	for( i = 2; i < Size - 2; i++ ){
		for( j = 2; j < Size - 2; j++ ){
			
		   flag = 0;
			
			if( afterThresh[i][j] == 127 ){
				
				if( afterThresh[i-1][j-1]>127 && afterThresh[i+1][j+1]<127 )
					flag = 1;
				if( afterThresh[i-1][j]>127 && afterThresh[i+1][j]<127 )
					flag = 1;
				if( afterThresh[i-1][j+1]>127 && afterThresh[i+1][j-1]<127 )
					flag = 1;
				if( afterThresh[i][j-1]>127 && afterThresh[i][j+1]<127 )
					flag = 1;
				if( afterThresh[i][j-1]<127 && afterThresh[i][j+1]>127 )
					flag = 1;
				if( afterThresh[i-1][j+1]<127 && afterThresh[i+1][j-1]>127 )
					flag = 1;
				if( afterThresh[i-1][j]<127 && afterThresh[i+1][j]>127 )
					flag = 1;				
				if( afterThresh[i-1][j-1]<127 && afterThresh[i+1][j+1]>127 )
					flag = 1;					
					
				if(flag==1)edgeMap[i][j] = 255;
				else edgeMap[i][j] = 0;
			}
			else edgeMap[i][j] = 0;
		
			
			
		}	//		End-2-for
	}	//		End-1-for
		
		

						/********************************************/
						/******                                ******/
						/******    write image to "afterGauss.raw"  ******/
						/******                                ******/
						/********************************************/		
	if (  !(file=fopen("afterGauss.raw","wb"))  ){
		cout<<"Cannot open file!"<<endl;
		exit(1);
	}
	else{
		fwrite(afterGauss, sizeof(unsigned char), Size*Size, file);
		fclose(file);
	}
	
	
						/********************************************/
						/******                                ******/
						/*****write image to "afterSecOrder.raw"  ******/
						/******                                ******/
						/********************************************/
	if (  !(file=fopen("afterSecOrder.raw","wb"))  ){
		cout<<"Cannot open file!"<<endl;
		exit(1);
	}
	else{
		fwrite(afterSecOrder, sizeof(unsigned char), Size*Size, file);
		fclose(file);
	}

						/********************************************/
						/******                                ******/
						/****write image to "afterThresh.raw"  ******/
						/******                                ******/
						/********************************************/
	if (  !(file=fopen("afterThresh.raw","wb"))  )	{
		cout<<"Cannot open file!"<<endl;
		exit(1);
	}
	else{
		fwrite(afterThresh, sizeof(unsigned char), Size*Size, file);
		fclose(file);
	}

						/********************************************/
						/******                                ******/
						/****write image to "edgeMap.raw"  ******/
						/******                                ******/
						/********************************************/	
	if (  !(file=fopen("edgeMap.raw","wb"))  ){
		cout<<"Cannot open file!"<<endl;
		exit(1);
	}
	else{
		fwrite(edgeMap, sizeof(unsigned char), Size*Size, file);
		fclose(file);
	}
	
		/********************************************/		



	system("PAUSE");
	exit(0);
}

