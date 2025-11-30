/*
 * grayscale.c
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <io.h>
#include <system.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>


//STOCKER LES VARIABLES SUR LE STACK ET NON SUR LE HEAP (avec malloc) !

unsigned char *grayscale_array;
int grayscale_width = 0;
int grayscape_height = 0;
static int gray_scale_LUT[USHRT_MAX]; //stocke la conversion rgb to gray dans une LUT


void init_grayscale_arrays(int width , int height)
{
	grayscale_array = (unsigned char *) malloc(width*height);
}

void conv_grayscale(void *picture,
		            int width,
		            int height) {
	int x,y,gray;
	unsigned short *pixels = (unsigned short *)picture , rgb;
	grayscale_width = width;
	grayscape_height = height;
	if (grayscale_array != NULL)
		free(grayscale_array);
	//grayscale_array = (unsigned char *) malloc(width*height);
	for (y = 0 ; y < height ; y++) {
		for (x = 0 ; x < width ; x++) {
			//rgb = pixels[y*width+x];
			/*gray = (((rgb>>11)&0x1F)<<3)*21; // red part
			gray += (((rgb>>5)&0x3F)<<2)*72; // green part
			gray += (((rgb>>0)&0x1F)<<3)*7; // blue part
			//gray /= 100;
			gray = gray >> 7;*/
			//gray = ALT_CI_CI_GRAYSCALE_0(rgb, 0x00);
			grayscale_array[y*width+x] = ALT_CI_CI_GRAYSCALE_0(pixels[y*width+x], 0x00);
			//IOWR_8DIRECT(grayscale_array,y*width+x,gray);
		}
	}
}

void conv_grayscale_with_subimg(void *src_picture, int x0_subimg,
								int y0_subimg, int w_subimg, int h_subimg,
								int width, int height)
{
	unsigned short *src = (unsigned short *)src_picture;
	int x,y;

	for (y = 0; y < h_subimg; y++) {

		//y0_subimg 	= numero de la ligne
		//x0_subimg 	= l'endroit sur la ligne
		const unsigned short *row_src = src + (y0_subimg + y) * width + x0_subimg;
		unsigned char *row_dst = grayscale_array + (y0_subimg + y) * width + x0_subimg;

		for (int x = 0; x < w_subimg; x += 32) {

		    row_dst[0]  = ALT_CI_CI_GRAYSCALE_0(row_src[0],  0x00);
		    row_dst[1]  = ALT_CI_CI_GRAYSCALE_0(row_src[1],  0x00);
		    row_dst[2]  = ALT_CI_CI_GRAYSCALE_0(row_src[2],  0x00);
		    row_dst[3]  = ALT_CI_CI_GRAYSCALE_0(row_src[3],  0x00);

		    row_dst[4]  = ALT_CI_CI_GRAYSCALE_0(row_src[4],  0x00);
		    row_dst[5]  = ALT_CI_CI_GRAYSCALE_0(row_src[5],  0x00);
		    row_dst[6]  = ALT_CI_CI_GRAYSCALE_0(row_src[6],  0x00);
		    row_dst[7]  = ALT_CI_CI_GRAYSCALE_0(row_src[7],  0x00);

		    row_dst[8]  = ALT_CI_CI_GRAYSCALE_0(row_src[8],  0x00);
		    row_dst[9]  = ALT_CI_CI_GRAYSCALE_0(row_src[9],  0x00);
		    row_dst[10] = ALT_CI_CI_GRAYSCALE_0(row_src[10], 0x00);
		    row_dst[11] = ALT_CI_CI_GRAYSCALE_0(row_src[11], 0x00);

		    row_dst[12] = ALT_CI_CI_GRAYSCALE_0(row_src[12], 0x00);
		    row_dst[13] = ALT_CI_CI_GRAYSCALE_0(row_src[13], 0x00);
		    row_dst[14] = ALT_CI_CI_GRAYSCALE_0(row_src[14], 0x00);
		    row_dst[15] = ALT_CI_CI_GRAYSCALE_0(row_src[15], 0x00);

		    // Les 16 suivants (pour faire 32)
		    row_dst[16] = ALT_CI_CI_GRAYSCALE_0(row_src[16], 0x00);
		    row_dst[17] = ALT_CI_CI_GRAYSCALE_0(row_src[17], 0x00);
		    row_dst[18] = ALT_CI_CI_GRAYSCALE_0(row_src[18], 0x00);
		    row_dst[19] = ALT_CI_CI_GRAYSCALE_0(row_src[19], 0x00);

		    row_dst[20] = ALT_CI_CI_GRAYSCALE_0(row_src[20], 0x00);
		    row_dst[21] = ALT_CI_CI_GRAYSCALE_0(row_src[21], 0x00);
		    row_dst[22] = ALT_CI_CI_GRAYSCALE_0(row_src[22], 0x00);
		    row_dst[23] = ALT_CI_CI_GRAYSCALE_0(row_src[23], 0x00);

		    row_dst[24] = ALT_CI_CI_GRAYSCALE_0(row_src[24], 0x00);
		    row_dst[25] = ALT_CI_CI_GRAYSCALE_0(row_src[25], 0x00);
		    row_dst[26] = ALT_CI_CI_GRAYSCALE_0(row_src[26], 0x00);
		    row_dst[27] = ALT_CI_CI_GRAYSCALE_0(row_src[27], 0x00);

		    row_dst[28] = ALT_CI_CI_GRAYSCALE_0(row_src[28], 0x00);
		    row_dst[29] = ALT_CI_CI_GRAYSCALE_0(row_src[29], 0x00);
		    row_dst[30] = ALT_CI_CI_GRAYSCALE_0(row_src[30], 0x00);
		    row_dst[31] = ALT_CI_CI_GRAYSCALE_0(row_src[31], 0x00);


		    row_dst += 32;
		    row_src += 32;
		}

	}
}



//Stockage de la conversion RGB to GRAY dans une LUT
//	--> le calcul de conversion se fera donc pas pendant l'acquisition de l'image

void build_LUT_rgb_to_gray(int width, int height)
{
	unsigned short pixel_rgb;
	for (pixel_rgb = 0 ; pixel_rgb < USHRT_MAX ; pixel_rgb++) {
		/*gray_scale_LUT[pixel_rgb] = (((pixel_rgb>>11)&0x1F)<<3)*21;
		gray_scale_LUT[pixel_rgb] += (((pixel_rgb>>5)&0x3F)<<2)*72;
		gray_scale_LUT[pixel_rgb] += (((pixel_rgb>>0)&0x1F)<<3)*7;
		gray_scale_LUT[pixel_rgb] = gray_scale_LUT[pixel_rgb]/100;*/

		gray_scale_LUT[pixel_rgb] = ALT_CI_CI_GRAYSCALE_0(pixel_rgb, 0x00);
	}

	grayscale_width = width;
	grayscape_height = height;
	grayscale_array = (unsigned char *)malloc(width*height); //alloue de la memoire pour avoir acces ailleurs (sur le Stack impossible)
}															 //possiblement stocke sur le cache

void conv_grayscale_LUT(void *picture, int width, int height) {

	int x,y,gray;
	unsigned short *pixels = (unsigned short *)picture , rgb;
	//grayscale_width = width;
	//grayscape_height = height;


	if (grayscale_array != NULL)
		free(grayscale_array);

	//grayscale_array = (unsigned char *) malloc(width*height);


	for (y = 0 ; y < height ; y++) {
		for (x = 0 ; x < width ; x++) {
			//plus de calculs de conversion, juste des lecture d'un tableau avec LUT
			rgb = pixels[y*width+x];
			gray = gray_scale_LUT[rgb];
			IOWR_8DIRECT(grayscale_array,y*width+x,gray);
		}
	}
}

void conv_grayscale_with_subimg_LUT(void *src_picture, int x0_subimg,
								int y0_subimg, int w_subimg, int h_subimg,
								int width, int height) {

	unsigned short *src = (unsigned short *)src_picture;
	int x,y;

	for (y = 0; y < h_subimg; y++) {

		//y0_subimg 	= numero de la ligne
		//x0_subimg 	= l'endroit sur la ligne
		const unsigned short *row_src = src + (y0_subimg + y) * width + x0_subimg;
		unsigned char *row_dst = grayscale_array + (y0_subimg + y) * width + x0_subimg;

		for (x = 0; x < w_subimg; x++) {
			//plus de calculs de conversion, juste des lecture d'un tableau avec LUT
			row_dst[x] = gray_scale_LUT[row_src[x]];
		}
	}
}


int get_grayscale_width() {
	return grayscale_width;
}

int get_grayscale_height() {
	return grayscape_height;
}

unsigned char *get_grayscale_picture() {
	return grayscale_array;
}


