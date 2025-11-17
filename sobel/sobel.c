/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"

const char gx_array[3][3] = {{-1,0,1},
                             {-2,0,2},
                             {-1,0,1}};
const char gy_array[3][3] = { {1, 2, 1},
                              {0, 0, 0},
                             {-1,-2,-1}};

short *sobel_x_result;
short *sobel_y_result;
unsigned short *sobel_rgb565;
unsigned char *sobel_result;
int sobel_width;
int sobel_height;

void init_sobel_arrays(int width , int height) {
	printf("Camera resolution - width : %d, height : %d \n", width, height);
	int loop;
	sobel_width = width;
	sobel_height = height;
	if (sobel_x_result != NULL)
		free(sobel_x_result);
	sobel_x_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_y_result != NULL)
		free(sobel_y_result);
	sobel_y_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)malloc(width*height*sizeof(unsigned char));
	if (sobel_rgb565 != NULL)
		free(sobel_rgb565);
	sobel_rgb565 = (unsigned short *)malloc(width*height*sizeof(unsigned short));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_x_result[loop] = 0;
		sobel_y_result[loop] = 0;
		sobel_result[loop] = 0;
		sobel_rgb565[loop] = 0;
	}
}


//Au lieu de parcourir 3 fois l'images (1 pour gx, 1 pour gy, 1 pour treshold) --> on parcourt une fois l'image
void sobel_complete(unsigned char *source, short threshold)
{
	int img_height = sobel_height-1;
	int img_width = sobel_width-1;
	int x,y, x_minus1, x0, x_plus1, array_index, y_minus1, y0, y_plus1;
	unsigned char* row_ym1;
	unsigned char* row_y0;
	unsigned char* row_yp1;
	short gx;
	short gy;
	short sum;

	for (y = 1 ; y < img_height ; y++) {
		//Pré-calcul des indices y (sinon on recalcul à chaque fois dans boucle x...)
		y_minus1 = (y - 1) * sobel_width;
		y0       = y * sobel_width;
		y_plus1  = (y + 1) * sobel_width;

		//On récupère une seule fois le pointeur vers chaque ligne correspondant à y-1, y et y+1
		//Ces adresses restent constantes dans la boucle x
		row_ym1 = &source[(y - 1) * sobel_width];
		row_y0  = &source[y * sobel_width];
		row_yp1 = &source[(y + 1) * sobel_width];

		for (x = 1 ; x < img_width ; x++) {
			//calcul x-1, x, x+1 une seule fois par itération de x
			x_minus1 = x - 1;
			x0       = x;
			x_plus1  = x + 1;

			//On parcours les colonnes correspondant à x-1, x, x+1 pour chaque ligne y-1, y, y+1 --> convolue le filtre de Sobel sur l'image
			//Optimisation : En enlevant multiplication avec tableau matrice de sobel
							//--> 2500 cycles/pixel à 1700 cycles/pixel
			gx =
				- row_ym1[x_minus1] + row_ym1[x_plus1]
				- ((row_y0[x_minus1] << 1) + (row_y0[x_plus1] << 1))
				- row_yp1[x_minus1] + row_yp1[x_plus1];

			gy =
			    + row_ym1[x_minus1] + (row_ym1[x0] << 1) + row_ym1[x_plus1]
				- row_yp1[x_minus1] - (row_yp1[x0] << 1) - row_yp1[x_plus1];

			array_index = y0 + x;

			//Applique un treshold
			sum = (gx < 0 ? -gx : gx) + (gy < 0 ? -gy : gy);
			sobel_result[array_index] = (sum > threshold) ? 0xFF : 0;
		}
	}
}

//local variable, on va chercher une seule fois les valeurs de la matrice de Sobel en mémoire
/*const char gx_00 = gx_array[0][0]; //-1
	const char gx_01 = gx_array[0][1]; //0
	const char gx_02 = gx_array[0][2]; //1
	const char gx_10 = gx_array[1][0]; //-2
	const char gx_11 = gx_array[1][1]; //0
	const char gx_12 = gx_array[1][2]; //2
	const char gx_20 = gx_array[2][0]; //-1
	const char gx_21 = gx_array[2][1]; //0
	const char gx_22 = gx_array[2][2]; //1

	const char gy_00 = gy_array[0][0]; //1
	const char gy_01 = gy_array[0][1]; //2
	const char gy_02 = gy_array[0][2]; //1
	const char gy_10 = gy_array[1][0]; //0
	const char gy_11 = gy_array[1][1]; //0
	const char gy_12 = gy_array[1][2]; //0
	const char gy_20 = gy_array[2][0]; //-1
	const char gy_21 = gy_array[2][1]; //-2
	const char gy_22 = gy_array[2][2]; //-1*/

/*gx = gx_00*row_ym1[x_minus1] + gx_01*row_ym1[x0] + gx_02*row_ym1[x_plus1]
			   + gx_10*row_y0[x_minus1]  + gx_11*row_y0[x0]  + gx_12*row_y0[x_plus1]
			   + gx_20*row_yp1[x_minus1] + gx_21*row_yp1[x0] + gx_22*row_yp1[x_plus1];

			gy = gy_00*row_ym1[x_minus1] + gy_01*row_ym1[x0] + gy_02*row_ym1[x_plus1]
			   + gy_10*row_y0[x_minus1]  + gy_11*row_y0[x0]  + gy_12*row_y0[x_plus1]
			   + gy_20*row_yp1[x_minus1] + gy_21*row_yp1[x0] + gy_22*row_yp1[x_plus1];*/



	//sobel_result = sobel_result_ptr;
	/*int arrayindex;
	int x,y;
	short gx, gy;
	short sum,value;

	for (y = 1 ; y < (sobel_height-1) ; y++) {
		for (x = 1 ; x < (sobel_width-1) ; x++) {

			gx = 0;
			gy = 0;
			arrayindex = (y*sobel_width)+x;

			//convolution avec filtre de sobel sur x et y
			gx += gx_array[0][0] * source[(y - 1) * sobel_width + (x - 1)];
			gx += gx_array[0][1] * source[(y - 1) * sobel_width + (x)];
			gx += gx_array[0][2] * source[(y - 1) * sobel_width + (x + 1)];
			gx += gx_array[1][0] * source[(y) * sobel_width + (x - 1)];
			gx += gx_array[1][1] * source[(y) * sobel_width + (x)];
			gx += gx_array[1][2] * source[(y) * sobel_width + (x + 1)];
			gx += gx_array[2][0] * source[(y + 1) * sobel_width + (x - 1)];
			gx += gx_array[2][1] * source[(y + 1) * sobel_width + (x)];
			gx += gx_array[2][2] * source[(y + 1) * sobel_width + (x + 1)];

			gy += gy_array[0][0] * source[(y - 1) * sobel_width + (x - 1)];
			gy += gy_array[0][1] * source[(y - 1) * sobel_width + (x)];
			gy += gy_array[0][2] * source[(y - 1) * sobel_width + (x + 1)];
			gy += gy_array[1][0] * source[(y) * sobel_width + (x - 1)];
			gy += gy_array[1][1] * source[(y) * sobel_width + (x)];
			gy += gy_array[1][2] * source[(y) * sobel_width + (x + 1)];
			gy += gy_array[2][0] * source[(y + 1) * sobel_width + (x - 1)];
			gy += gy_array[2][1] * source[(y + 1) * sobel_width + (x)];
			gy += gy_array[2][2] * source[(y + 1) * sobel_width + (x + 1)];

			//sauvegarde du resultat
			sobel_x_result[arrayindex] = gx;
			sobel_y_result[arrayindex] = gy;

			//threshold
			sum = (gx < 0) ? -gx : gx;
			sum += (gy < 0) ? -gy : gy;
			sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
		}
	}*/


short sobel_mac( unsigned char *pixels,
                 int x,
                 int y,
                 const char *filter,
                 unsigned int width ) {
   /*short dy,dx;
   short result = 0;
   for (dy = -1 ; dy < 2 ; dy++) {
      for (dx = -1 ; dx < 2 ; dx++) {
         result += filter[(dy+1)*3+(dx+1)]*
                   pixels[(y+dy)*width+(x+dx)];
      }
   }
   return result;*/


//REMOVE INNER LOOP

	/*short dy,dx;
	   short result = 0;
	   for (dy = -1 ; dy < 2 ; dy++) {

		   result += filter[(dy+1)*3+(0)]*
	                 pixels[(y+dy)*width+(x-1)];
		   result += filter[(dy+1)*3+(1)]*
		   	         pixels[(y+dy)*width+(x+0)];
		   result += filter[(dy+1)*3+(2)]*
		   		   	 pixels[(y+dy)*width+(x+1)];

	   }
	   return result;*/

//NO LOOP

	   short result = 0;


	   result += filter[(0)*3+(0)]*
				 pixels[(y-1)*width+(x-1)];
	   result += filter[(0)*3+(1)]*
				 pixels[(y-1)*width+(x+0)];
	   result += filter[(0)*3+(2)]*
				 pixels[(y-1)*width+(x+1)];


	   result += filter[(1)*3+(0)]*
				 pixels[(y)*width+(x-1)];
	   result += filter[(1)*3+(1)]*
				 pixels[(y)*width+(x+0)];
	   result += filter[(1)*3+(2)]*
				 pixels[(y)*width+(x+1)];


		result += filter[(2)*3+(0)]*
				 pixels[(y+1)*width+(x-1)];
	   result += filter[(2)*3+(1)]*
				 pixels[(y+1)*width+(x+0)];
	   result += filter[(2)*3+(2)]*
				 pixels[(y+1)*width+(x+1)];



	   return result;


}

void sobel_x( unsigned char *source ) {
   int x,y;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
         sobel_x_result[y*sobel_width+x] = sobel_mac(source,x,y,gx_array,sobel_width);
      }
   }
}

void sobel_x_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gx_array,sobel_width);
          sobel_x_result[y*sobel_width+x] = result;
          if (result < 0) {
        	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
          } else {
        	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
          }
      }
   }
}

void sobel_y( unsigned char *source ) {
   int x,y;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
         sobel_y_result[y*sobel_width+x] = sobel_mac(source,x,y,gy_array,sobel_width);
      }
   }
}

void sobel_y_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gy_array,sobel_width);
         sobel_y_result[y*sobel_width+x] = result;
         if (result < 0) {
       	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
         } else {
       	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
         }
      }
   }
}

void sobel_threshold(short threshold) {
	int x,y,arrayindex;
	short sum,value;
	for (y = 1 ; y < (sobel_height-1) ; y++) {
		for (x = 1 ; x < (sobel_width-1) ; x++) {
			arrayindex = (y*sobel_width)+x;
			value = sobel_x_result[arrayindex];
			sum = (value < 0) ? -value : value;
			value = sobel_y_result[arrayindex];
			sum += (value < 0) ? -value : value;
			sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
		}
	}
}

unsigned short *GetSobel_rgb() {
	return sobel_rgb565;
}

unsigned char *GetSobelResult() {
	return sobel_result;
}
