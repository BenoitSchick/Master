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


//STOCKER LES VARIABLES SUR LE STACK ET NON SUR LE HEAP (avec malloc) !

unsigned char *grayscale_array;
int grayscale_width = 0;
int grayscape_height = 0;
static int gray_scale_LUT[USHRT_MAX]; //stocke la conversion rgb to gray dans une LUT



//Stockage de la conversion RGB to GRAY dans une LUT
//	--> le calcul de conversion se fera donc pas pendant l'acquisition de l'image
static int gray_scale_LUT[USHRT_MAX]; //stocke la conversion rgb to gray dans une LUT

void build_LUT_rgb_to_gray(int width, int height)
{
	unsigned short pixel_rgb;
	for (pixel_rgb = 0 ; pixel_rgb < USHRT_MAX ; pixel_rgb++) {
		gray_scale_LUT[pixel_rgb] = (((pixel_rgb>>11)&0x1F)<<3)*21;
		gray_scale_LUT[pixel_rgb] += (((pixel_rgb>>5)&0x3F)<<2)*72;
		gray_scale_LUT[pixel_rgb] += (((pixel_rgb>>0)&0x1F)<<3)*7;
		gray_scale_LUT[pixel_rgb] = gray_scale_LUT[pixel_rgb]/100;
	}

	grayscale_width = width;
	grayscape_height = height;
	grayscale_array = (unsigned char *)malloc(width*height); //alloue de la memoire pour avoir acces ailleurs (sur le Stack impossible)
}															 //possiblement stocke sur le cache

void conv_grayscale(void *picture, int width, int height) {

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

void conv_grayscale_with_subimg(void *src_picture, int x0_subimg,
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


