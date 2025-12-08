/*
 * grayscale.h
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#ifndef GRAYSCALE_H_
#define GRAYSCALE_H_


/**
 * @brief - Convert sub-images in grayscale color
 * 
 * @param src_picture pointer to the beginning of the full image to convert
 * @param x0_subimg top left corner of the sub-image on the x-axis 
 * @param y0_subimg top left corner of the sub-image on the y-axis
 * @param w_subimg width of the sub-image
 * @param h_subimg height of the sub-image
 * @param dest_gray_subimg pointer on the sub-image converted to grayscale
 *
 * @return void
 */

void init_grayscale_arrays(int width , int height);
void conv_grayscale_with_subimg_MultBits(void *src_picture, int x0_subimg,
								int y0_subimg, int w_subimg, int h_subimg,
								int width, int height);
void conv_grayscale_with_subimg(void *src_picture, int x0_subimg,
								int y0_subimg, int w_subimg, int h_subimg,
								int width, int height);

void conv_grayscale(void *picture,
		            int width,
		            int height);

void build_LUT_rgb_to_gray(int width, int height);
void conv_grayscale_LUT(void *picture,
		            int width,
		            int height);
void conv_grayscale_with_subimg_LUT(void *src_picture, int x0_subimg,
								int y0_subimg, int w_subimg, int h_subimg,
								int width, int height);
int get_grayscale_width();
int get_grayscale_height();
unsigned char *get_grayscale_picture();

#endif /* GRAYSCALE_H_ */
