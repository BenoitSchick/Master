/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include <stdint.h>
#include <system.h>

const char gx_array[3][3] = {{-1,0,1},
                             {-2,0,2},
                             {-1,0,1}};
const char gy_array[3][3] = { {1, 2, 1},
                              {0, 0, 0},
                             {-1,-2,-1}};


const char gx_00_array[3] = {{-1,0,1}};
const char gx_01_array[3] = {{1},
							 {2},
							 {1}};

const char gy_00_array[3] = {{1,2,1}};
const char gy_01_array[3] = {{-1},
							 {0},
							 {1}};



//STOCKER LES VARIABLES SUR LE STACK ET NON SUR LE HEAP (avec malloc) !
//VARIABLES INTERMEDIAIRE EN 32 BITS PLUTOT QUE 16 BITS ! moins d'operation avec NIOS II

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

	// sobel result stockï¿½ hors du cache --> 140 cycles (cache) ï¿½ 79 cycles (hors cache)

	//sobel_result = (unsigned char*)alt_uncached_malloc(width*height*sizeof(unsigned char));
	memset(sobel_result, 0, width*height*sizeof(unsigned char));

	//sobel result stocke dans le cache

	//__attribute__ ((section (".tcm_data")));
	sobel_result = (unsigned char*)malloc(width*height*sizeof(unsigned char));

	//sobel result sur Stack --> 120 cycles/pixel ==> plus efficace que malloc
}


//Au lieu de parcourir 3 fois l'images (1 pour gx, 1 pour gy, 1 pour treshold) --> on parcourt une fois l'image
void sobel_complete(unsigned char *source, short threshold)
{
	//Stocker dans variables 32bits --> moins d'operation avec NIOS II
	int img_height = sobel_height-1;
	int img_width = sobel_width-1;
	int x,y, x_minus1, x0, x_plus1, array_index, y_minus1, y0, y_plus1;
	unsigned char* row_ym1;
	unsigned char* row_y0;
	unsigned char* row_yp1;
	int gx;
	int gy;
	int sum;


	for (y = 1 ; y < img_height ; y++) {
		//Prï¿½-calcul des indices y (sinon on recalcul ï¿½ chaque fois dans boucle x...)
		y_minus1 = (y - 1) * sobel_width;
		y0       = y * sobel_width;
		y_plus1  = (y + 1) * sobel_width;

		//On rï¿½cupï¿½re une seule fois le pointeur vers chaque ligne correspondant ï¿½ y-1, y et y+1
		//Ces adresses restent constantes dans la boucle x
		row_ym1 = &source[y_minus1];
		row_y0  = &source[y0];
		row_yp1 = &source[y_plus1];


		for (x = 1 ; x < img_width ; x++) {
			//calcul x-1, x, x+1 une seule fois par itï¿½ration de x
			x_minus1 = x - 1;
			x0       = x;
			x_plus1  = x + 1;

			//On parcours les colonnes correspondant ï¿½ x-1, x, x+1 pour chaque ligne y-1, y, y+1 --> convolue le filtre de Sobel sur l'image
			//Optimisation : Enlever multiplication avec tableau matrice de sobel

			gx =
			    - row_ym1[x_minus1] + row_ym1[x_plus1]
			    - (row_y0[x_minus1] << 1) + (row_y0[x_plus1] << 1)
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

void sobel_complete_with_subimg(unsigned char *source, short threshold,
								int x0_subimg, int y0_subimg, int w_subimg, int h_subimg)
{
	int x,y, x_minus2, x_minus1, x0, x_plus1, array_index, y_minus2, y_minus1, y0, y_plus1;
	unsigned char* row_ym1;
	unsigned char* row_y0;
	unsigned char* row_yp1;

	int gx;
	int gy;
	int sum;

	//definition des limites
	int y_start = y0_subimg;
	int y_end = y0_subimg+h_subimg;
	int x_start = x0_subimg;
	int x_end = x0_subimg+w_subimg;


	for (y = y_start ; y < y_end ; y+=1) {
		//pre-calcul des indices

		//y_minus2 = (y - 2) * sobel_width;
		y_minus1 = (y - 1) * sobel_width;
		y0       = y * sobel_width;
		y_plus1  = (y + 1) * sobel_width;

		row_ym1 = &source[y_minus1];
		row_y0  = &source[y0];
		row_yp1 = &source[y_plus1];

		//unsigned char *dst_bef2 = &sobel_result[y_minus2];
		//unsigned char *dst_bef1 = &sobel_result[y_minus1];
		//unsigned char *dst_next = &sobel_result[y_plus1];

		unsigned char *dst = &sobel_result[y0]; //selectionne la ligne y
											//	--> parcourt les pixels de x_start à x_end de la ligne y

		for (x = x_start ; x < x_end ; x+=1) {

			//x_minus2 = x - 2;
			x_minus1 = x - 1;
			x_plus1  = x + 1;


			uint32_t dataa = 	(((uint32_t)(row_ym1[x_minus1])) << 24) |
								(((uint32_t)(row_ym1[x_plus1])) << 16) |
								(((uint32_t)(row_y0[x_minus1])) << 8)  |
								((uint32_t)(row_y0[x_plus1]));

			uint32_t datab =	(((uint32_t)(row_yp1[x_minus1])) << 24) |
							 	(((uint32_t)(row_yp1[x_plus1])) << 16) |
								(((uint32_t)row_ym1[x])   << 8)  |
								((uint32_t)row_yp1[x]);

			dst[x] = ALT_CI_CI_SOBEL_0(dataa, datab);


			//convolution matrice de sobel avec image
			/*gx =
				- row_ym1[x_minus1] + row_ym1[x_plus1]
				- (row_y0[x_minus1] << 1) + (row_y0[x_plus1] << 1)
				- row_yp1[x_minus1] + row_yp1[x_plus1];

			gy =
				+ row_ym1[x_minus1] + (row_ym1[x] << 1) + row_ym1[x_plus1]
				- row_yp1[x_minus1] - (row_yp1[x] << 1) - row_yp1[x_plus1];*/

			/*//Applique un treshold
			sum = (gx < 0 ? -gx : gx) + (gy < 0 ? -gy : gy);

			//Result sobel
			unsigned char val = (sum > threshold) ? 0xFF : 0;
			dst[x] = val;*/







		// Interpolation

			//unsigned char val_inter = (dst_bef[x_minus2] + dst[x_minus2] + dst_bef[x] + dst[x]) >> 2;

			/*if(y != 0) {
				if(x != 0) {
					unsigned char val_inter =
							(sobel_result[(y-2)*sobel_width + x-2] +
							sobel_result[y*sobel_width + x-2] +
							sobel_result[(y-2)*sobel_width + x] +
							sobel_result[y*sobel_width + x]) >> 2;

					//sobel_result[(y-1)*sobel_width + x] = val_inter;
					//sobel_result[(y)*sobel_width + x-1] = val_inter;
					sobel_result[(y-1)*sobel_width + x-1] = val_inter;
					//sobel_result[(y-2)*sobel_width + x-1] = val_inter;
					//sobel_result[(y-1)*sobel_width + x-2] = val_inter;
				}
			}*/



		//Interpolation

			//sobel_result[y0 + x] = val;


			/*if (x+1 < x_end)       sobel_result[y0 + x + 1] = val;        // copie pixel ï¿½ droite
			if (y+1 < y_end) {
				sobel_result[(y+1)*sobel_width + x] = val;             // pixel en dessous
				if (x+1 < x_end) sobel_result[(y+1)*sobel_width + x+1] = val; // diagonale
			}*/
		}
	}
}


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
