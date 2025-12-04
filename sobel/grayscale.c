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
	//__attribute__ ((section (".tcm_data")));
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

		row_dst[0]=ALT_CI_CI_GRAYSCALE_0(row_src[0],0x00);
		row_dst[1]=ALT_CI_CI_GRAYSCALE_0(row_src[1],0x00);
		row_dst[2]=ALT_CI_CI_GRAYSCALE_0(row_src[2],0x00);
		row_dst[3]=ALT_CI_CI_GRAYSCALE_0(row_src[3],0x00);
		row_dst[4]=ALT_CI_CI_GRAYSCALE_0(row_src[4],0x00);
		row_dst[5]=ALT_CI_CI_GRAYSCALE_0(row_src[5],0x00);
		row_dst[6]=ALT_CI_CI_GRAYSCALE_0(row_src[6],0x00);
		row_dst[7]=ALT_CI_CI_GRAYSCALE_0(row_src[7],0x00);
		row_dst[8]=ALT_CI_CI_GRAYSCALE_0(row_src[8],0x00);
		row_dst[9]=ALT_CI_CI_GRAYSCALE_0(row_src[9],0x00);
		row_dst[10]=ALT_CI_CI_GRAYSCALE_0(row_src[10],0x00);
		row_dst[11]=ALT_CI_CI_GRAYSCALE_0(row_src[11],0x00);
		row_dst[12]=ALT_CI_CI_GRAYSCALE_0(row_src[12],0x00);
		row_dst[13]=ALT_CI_CI_GRAYSCALE_0(row_src[13],0x00);
		row_dst[14]=ALT_CI_CI_GRAYSCALE_0(row_src[14],0x00);
		row_dst[15]=ALT_CI_CI_GRAYSCALE_0(row_src[15],0x00);
		row_dst[16]=ALT_CI_CI_GRAYSCALE_0(row_src[16],0x00);
		row_dst[17]=ALT_CI_CI_GRAYSCALE_0(row_src[17],0x00);
		row_dst[18]=ALT_CI_CI_GRAYSCALE_0(row_src[18],0x00);
		row_dst[19]=ALT_CI_CI_GRAYSCALE_0(row_src[19],0x00);
		row_dst[20]=ALT_CI_CI_GRAYSCALE_0(row_src[20],0x00);
		row_dst[21]=ALT_CI_CI_GRAYSCALE_0(row_src[21],0x00);
		row_dst[22]=ALT_CI_CI_GRAYSCALE_0(row_src[22],0x00);
		row_dst[23]=ALT_CI_CI_GRAYSCALE_0(row_src[23],0x00);
		row_dst[24]=ALT_CI_CI_GRAYSCALE_0(row_src[24],0x00);
		row_dst[25]=ALT_CI_CI_GRAYSCALE_0(row_src[25],0x00);
		row_dst[26]=ALT_CI_CI_GRAYSCALE_0(row_src[26],0x00);
		row_dst[27]=ALT_CI_CI_GRAYSCALE_0(row_src[27],0x00);
		row_dst[28]=ALT_CI_CI_GRAYSCALE_0(row_src[28],0x00);
		row_dst[29]=ALT_CI_CI_GRAYSCALE_0(row_src[29],0x00);
		row_dst[30]=ALT_CI_CI_GRAYSCALE_0(row_src[30],0x00);
		row_dst[31]=ALT_CI_CI_GRAYSCALE_0(row_src[31],0x00);
		row_dst[32]=ALT_CI_CI_GRAYSCALE_0(row_src[32],0x00);
		row_dst[33]=ALT_CI_CI_GRAYSCALE_0(row_src[33],0x00);
		row_dst[34]=ALT_CI_CI_GRAYSCALE_0(row_src[34],0x00);
		row_dst[35]=ALT_CI_CI_GRAYSCALE_0(row_src[35],0x00);
		row_dst[36]=ALT_CI_CI_GRAYSCALE_0(row_src[36],0x00);
		row_dst[37]=ALT_CI_CI_GRAYSCALE_0(row_src[37],0x00);
		row_dst[38]=ALT_CI_CI_GRAYSCALE_0(row_src[38],0x00);
		row_dst[39]=ALT_CI_CI_GRAYSCALE_0(row_src[39],0x00);
		row_dst[40]=ALT_CI_CI_GRAYSCALE_0(row_src[40],0x00);
		row_dst[41]=ALT_CI_CI_GRAYSCALE_0(row_src[41],0x00);
		row_dst[42]=ALT_CI_CI_GRAYSCALE_0(row_src[42],0x00);
		row_dst[43]=ALT_CI_CI_GRAYSCALE_0(row_src[43],0x00);
		row_dst[44]=ALT_CI_CI_GRAYSCALE_0(row_src[44],0x00);
		row_dst[45]=ALT_CI_CI_GRAYSCALE_0(row_src[45],0x00);
		row_dst[46]=ALT_CI_CI_GRAYSCALE_0(row_src[46],0x00);
		row_dst[47]=ALT_CI_CI_GRAYSCALE_0(row_src[47],0x00);
		row_dst[48]=ALT_CI_CI_GRAYSCALE_0(row_src[48],0x00);
		row_dst[49]=ALT_CI_CI_GRAYSCALE_0(row_src[49],0x00);
		row_dst[50]=ALT_CI_CI_GRAYSCALE_0(row_src[50],0x00);
		row_dst[51]=ALT_CI_CI_GRAYSCALE_0(row_src[51],0x00);
		row_dst[52]=ALT_CI_CI_GRAYSCALE_0(row_src[52],0x00);
		row_dst[53]=ALT_CI_CI_GRAYSCALE_0(row_src[53],0x00);
		row_dst[54]=ALT_CI_CI_GRAYSCALE_0(row_src[54],0x00);
		row_dst[55]=ALT_CI_CI_GRAYSCALE_0(row_src[55],0x00);
		row_dst[56]=ALT_CI_CI_GRAYSCALE_0(row_src[56],0x00);
		row_dst[57]=ALT_CI_CI_GRAYSCALE_0(row_src[57],0x00);
		row_dst[58]=ALT_CI_CI_GRAYSCALE_0(row_src[58],0x00);
		row_dst[59]=ALT_CI_CI_GRAYSCALE_0(row_src[59],0x00);
		row_dst[60]=ALT_CI_CI_GRAYSCALE_0(row_src[60],0x00);
		row_dst[61]=ALT_CI_CI_GRAYSCALE_0(row_src[61],0x00);
		row_dst[62]=ALT_CI_CI_GRAYSCALE_0(row_src[62],0x00);
		row_dst[63]=ALT_CI_CI_GRAYSCALE_0(row_src[63],0x00);
		row_dst[64]=ALT_CI_CI_GRAYSCALE_0(row_src[64],0x00);
		row_dst[65]=ALT_CI_CI_GRAYSCALE_0(row_src[65],0x00);
		row_dst[66]=ALT_CI_CI_GRAYSCALE_0(row_src[66],0x00);
		row_dst[67]=ALT_CI_CI_GRAYSCALE_0(row_src[67],0x00);
		row_dst[68]=ALT_CI_CI_GRAYSCALE_0(row_src[68],0x00);
		row_dst[69]=ALT_CI_CI_GRAYSCALE_0(row_src[69],0x00);
		row_dst[70]=ALT_CI_CI_GRAYSCALE_0(row_src[70],0x00);
		row_dst[71]=ALT_CI_CI_GRAYSCALE_0(row_src[71],0x00);
		row_dst[72]=ALT_CI_CI_GRAYSCALE_0(row_src[72],0x00);
		row_dst[73]=ALT_CI_CI_GRAYSCALE_0(row_src[73],0x00);
		row_dst[74]=ALT_CI_CI_GRAYSCALE_0(row_src[74],0x00);
		row_dst[75]=ALT_CI_CI_GRAYSCALE_0(row_src[75],0x00);
		row_dst[76]=ALT_CI_CI_GRAYSCALE_0(row_src[76],0x00);
		row_dst[77]=ALT_CI_CI_GRAYSCALE_0(row_src[77],0x00);
		row_dst[78]=ALT_CI_CI_GRAYSCALE_0(row_src[78],0x00);
		row_dst[79]=ALT_CI_CI_GRAYSCALE_0(row_src[79],0x00);
		row_dst[80]=ALT_CI_CI_GRAYSCALE_0(row_src[80],0x00);
		row_dst[81]=ALT_CI_CI_GRAYSCALE_0(row_src[81],0x00);
		row_dst[82]=ALT_CI_CI_GRAYSCALE_0(row_src[82],0x00);
		row_dst[83]=ALT_CI_CI_GRAYSCALE_0(row_src[83],0x00);
		row_dst[84]=ALT_CI_CI_GRAYSCALE_0(row_src[84],0x00);
		row_dst[85]=ALT_CI_CI_GRAYSCALE_0(row_src[85],0x00);
		row_dst[86]=ALT_CI_CI_GRAYSCALE_0(row_src[86],0x00);
		row_dst[87]=ALT_CI_CI_GRAYSCALE_0(row_src[87],0x00);
		row_dst[88]=ALT_CI_CI_GRAYSCALE_0(row_src[88],0x00);
		row_dst[89]=ALT_CI_CI_GRAYSCALE_0(row_src[89],0x00);
		row_dst[90]=ALT_CI_CI_GRAYSCALE_0(row_src[90],0x00);
		row_dst[91]=ALT_CI_CI_GRAYSCALE_0(row_src[91],0x00);
		row_dst[92]=ALT_CI_CI_GRAYSCALE_0(row_src[92],0x00);
		row_dst[93]=ALT_CI_CI_GRAYSCALE_0(row_src[93],0x00);
		row_dst[94]=ALT_CI_CI_GRAYSCALE_0(row_src[94],0x00);
		row_dst[95]=ALT_CI_CI_GRAYSCALE_0(row_src[95],0x00);
		row_dst[96]=ALT_CI_CI_GRAYSCALE_0(row_src[96],0x00);
		row_dst[97]=ALT_CI_CI_GRAYSCALE_0(row_src[97],0x00);
		row_dst[98]=ALT_CI_CI_GRAYSCALE_0(row_src[98],0x00);
		row_dst[99]=ALT_CI_CI_GRAYSCALE_0(row_src[99],0x00);
		row_dst[100]=ALT_CI_CI_GRAYSCALE_0(row_src[100],0x00);
		row_dst[101]=ALT_CI_CI_GRAYSCALE_0(row_src[101],0x00);
		row_dst[102]=ALT_CI_CI_GRAYSCALE_0(row_src[102],0x00);
		row_dst[103]=ALT_CI_CI_GRAYSCALE_0(row_src[103],0x00);
		row_dst[104]=ALT_CI_CI_GRAYSCALE_0(row_src[104],0x00);
		row_dst[105]=ALT_CI_CI_GRAYSCALE_0(row_src[105],0x00);
		row_dst[106]=ALT_CI_CI_GRAYSCALE_0(row_src[106],0x00);
		row_dst[107]=ALT_CI_CI_GRAYSCALE_0(row_src[107],0x00);
		row_dst[108]=ALT_CI_CI_GRAYSCALE_0(row_src[108],0x00);
		row_dst[109]=ALT_CI_CI_GRAYSCALE_0(row_src[109],0x00);
		row_dst[110]=ALT_CI_CI_GRAYSCALE_0(row_src[110],0x00);
		row_dst[111]=ALT_CI_CI_GRAYSCALE_0(row_src[111],0x00);
		row_dst[112]=ALT_CI_CI_GRAYSCALE_0(row_src[112],0x00);
		row_dst[113]=ALT_CI_CI_GRAYSCALE_0(row_src[113],0x00);
		row_dst[114]=ALT_CI_CI_GRAYSCALE_0(row_src[114],0x00);
		row_dst[115]=ALT_CI_CI_GRAYSCALE_0(row_src[115],0x00);
		row_dst[116]=ALT_CI_CI_GRAYSCALE_0(row_src[116],0x00);
		row_dst[117]=ALT_CI_CI_GRAYSCALE_0(row_src[117],0x00);
		row_dst[118]=ALT_CI_CI_GRAYSCALE_0(row_src[118],0x00);
		row_dst[119]=ALT_CI_CI_GRAYSCALE_0(row_src[119],0x00);
		row_dst[120]=ALT_CI_CI_GRAYSCALE_0(row_src[120],0x00);
		row_dst[121]=ALT_CI_CI_GRAYSCALE_0(row_src[121],0x00);
		row_dst[122]=ALT_CI_CI_GRAYSCALE_0(row_src[122],0x00);
		row_dst[123]=ALT_CI_CI_GRAYSCALE_0(row_src[123],0x00);
		row_dst[124]=ALT_CI_CI_GRAYSCALE_0(row_src[124],0x00);
		row_dst[125]=ALT_CI_CI_GRAYSCALE_0(row_src[125],0x00);
		row_dst[126]=ALT_CI_CI_GRAYSCALE_0(row_src[126],0x00);
		row_dst[127]=ALT_CI_CI_GRAYSCALE_0(row_src[127],0x00);
		row_dst[128]=ALT_CI_CI_GRAYSCALE_0(row_src[128],0x00);
		row_dst[129]=ALT_CI_CI_GRAYSCALE_0(row_src[129],0x00);
		row_dst[130]=ALT_CI_CI_GRAYSCALE_0(row_src[130],0x00);
		row_dst[131]=ALT_CI_CI_GRAYSCALE_0(row_src[131],0x00);
		row_dst[132]=ALT_CI_CI_GRAYSCALE_0(row_src[132],0x00);
		row_dst[133]=ALT_CI_CI_GRAYSCALE_0(row_src[133],0x00);
		row_dst[134]=ALT_CI_CI_GRAYSCALE_0(row_src[134],0x00);
		row_dst[135]=ALT_CI_CI_GRAYSCALE_0(row_src[135],0x00);
		row_dst[136]=ALT_CI_CI_GRAYSCALE_0(row_src[136],0x00);
		row_dst[137]=ALT_CI_CI_GRAYSCALE_0(row_src[137],0x00);
		row_dst[138]=ALT_CI_CI_GRAYSCALE_0(row_src[138],0x00);
		row_dst[139]=ALT_CI_CI_GRAYSCALE_0(row_src[139],0x00);
		row_dst[140]=ALT_CI_CI_GRAYSCALE_0(row_src[140],0x00);
		row_dst[141]=ALT_CI_CI_GRAYSCALE_0(row_src[141],0x00);
		row_dst[142]=ALT_CI_CI_GRAYSCALE_0(row_src[142],0x00);
		row_dst[143]=ALT_CI_CI_GRAYSCALE_0(row_src[143],0x00);
		row_dst[144]=ALT_CI_CI_GRAYSCALE_0(row_src[144],0x00);
		row_dst[145]=ALT_CI_CI_GRAYSCALE_0(row_src[145],0x00);
		row_dst[146]=ALT_CI_CI_GRAYSCALE_0(row_src[146],0x00);
		row_dst[147]=ALT_CI_CI_GRAYSCALE_0(row_src[147],0x00);
		row_dst[148]=ALT_CI_CI_GRAYSCALE_0(row_src[148],0x00);
		row_dst[149]=ALT_CI_CI_GRAYSCALE_0(row_src[149],0x00);
		row_dst[150]=ALT_CI_CI_GRAYSCALE_0(row_src[150],0x00);
		row_dst[151]=ALT_CI_CI_GRAYSCALE_0(row_src[151],0x00);
		row_dst[152]=ALT_CI_CI_GRAYSCALE_0(row_src[152],0x00);
		row_dst[153]=ALT_CI_CI_GRAYSCALE_0(row_src[153],0x00);
		row_dst[154]=ALT_CI_CI_GRAYSCALE_0(row_src[154],0x00);
		row_dst[155]=ALT_CI_CI_GRAYSCALE_0(row_src[155],0x00);
		row_dst[156]=ALT_CI_CI_GRAYSCALE_0(row_src[156],0x00);
		row_dst[157]=ALT_CI_CI_GRAYSCALE_0(row_src[157],0x00);
		row_dst[158]=ALT_CI_CI_GRAYSCALE_0(row_src[158],0x00);
		row_dst[159]=ALT_CI_CI_GRAYSCALE_0(row_src[159],0x00);
		row_dst[160]=ALT_CI_CI_GRAYSCALE_0(row_src[160],0x00);
		row_dst[161]=ALT_CI_CI_GRAYSCALE_0(row_src[161],0x00);
		row_dst[162]=ALT_CI_CI_GRAYSCALE_0(row_src[162],0x00);
		row_dst[163]=ALT_CI_CI_GRAYSCALE_0(row_src[163],0x00);
		row_dst[164]=ALT_CI_CI_GRAYSCALE_0(row_src[164],0x00);
		row_dst[165]=ALT_CI_CI_GRAYSCALE_0(row_src[165],0x00);
		row_dst[166]=ALT_CI_CI_GRAYSCALE_0(row_src[166],0x00);
		row_dst[167]=ALT_CI_CI_GRAYSCALE_0(row_src[167],0x00);
		row_dst[168]=ALT_CI_CI_GRAYSCALE_0(row_src[168],0x00);
		row_dst[169]=ALT_CI_CI_GRAYSCALE_0(row_src[169],0x00);
		row_dst[170]=ALT_CI_CI_GRAYSCALE_0(row_src[170],0x00);
		row_dst[171]=ALT_CI_CI_GRAYSCALE_0(row_src[171],0x00);
		row_dst[172]=ALT_CI_CI_GRAYSCALE_0(row_src[172],0x00);
		row_dst[173]=ALT_CI_CI_GRAYSCALE_0(row_src[173],0x00);
		row_dst[174]=ALT_CI_CI_GRAYSCALE_0(row_src[174],0x00);
		row_dst[175]=ALT_CI_CI_GRAYSCALE_0(row_src[175],0x00);
		row_dst[176]=ALT_CI_CI_GRAYSCALE_0(row_src[176],0x00);
		row_dst[177]=ALT_CI_CI_GRAYSCALE_0(row_src[177],0x00);
		row_dst[178]=ALT_CI_CI_GRAYSCALE_0(row_src[178],0x00);
		row_dst[179]=ALT_CI_CI_GRAYSCALE_0(row_src[179],0x00);
		row_dst[180]=ALT_CI_CI_GRAYSCALE_0(row_src[180],0x00);
		row_dst[181]=ALT_CI_CI_GRAYSCALE_0(row_src[181],0x00);
		row_dst[182]=ALT_CI_CI_GRAYSCALE_0(row_src[182],0x00);
		row_dst[183]=ALT_CI_CI_GRAYSCALE_0(row_src[183],0x00);
		row_dst[184]=ALT_CI_CI_GRAYSCALE_0(row_src[184],0x00);
		row_dst[185]=ALT_CI_CI_GRAYSCALE_0(row_src[185],0x00);
		row_dst[186]=ALT_CI_CI_GRAYSCALE_0(row_src[186],0x00);
		row_dst[187]=ALT_CI_CI_GRAYSCALE_0(row_src[187],0x00);
		row_dst[188]=ALT_CI_CI_GRAYSCALE_0(row_src[188],0x00);
		row_dst[189]=ALT_CI_CI_GRAYSCALE_0(row_src[189],0x00);
		row_dst[190]=ALT_CI_CI_GRAYSCALE_0(row_src[190],0x00);
		row_dst[191]=ALT_CI_CI_GRAYSCALE_0(row_src[191],0x00);
		row_dst[192]=ALT_CI_CI_GRAYSCALE_0(row_src[192],0x00);
		row_dst[193]=ALT_CI_CI_GRAYSCALE_0(row_src[193],0x00);
		row_dst[194]=ALT_CI_CI_GRAYSCALE_0(row_src[194],0x00);
		row_dst[195]=ALT_CI_CI_GRAYSCALE_0(row_src[195],0x00);
		row_dst[196]=ALT_CI_CI_GRAYSCALE_0(row_src[196],0x00);
		row_dst[197]=ALT_CI_CI_GRAYSCALE_0(row_src[197],0x00);
		row_dst[198]=ALT_CI_CI_GRAYSCALE_0(row_src[198],0x00);
		row_dst[199]=ALT_CI_CI_GRAYSCALE_0(row_src[199],0x00);
		row_dst[200]=ALT_CI_CI_GRAYSCALE_0(row_src[200],0x00);
		row_dst[201]=ALT_CI_CI_GRAYSCALE_0(row_src[201],0x00);
		row_dst[202]=ALT_CI_CI_GRAYSCALE_0(row_src[202],0x00);
		row_dst[203]=ALT_CI_CI_GRAYSCALE_0(row_src[203],0x00);
		row_dst[204]=ALT_CI_CI_GRAYSCALE_0(row_src[204],0x00);
		row_dst[205]=ALT_CI_CI_GRAYSCALE_0(row_src[205],0x00);
		row_dst[206]=ALT_CI_CI_GRAYSCALE_0(row_src[206],0x00);
		row_dst[207]=ALT_CI_CI_GRAYSCALE_0(row_src[207],0x00);
		row_dst[208]=ALT_CI_CI_GRAYSCALE_0(row_src[208],0x00);
		row_dst[209]=ALT_CI_CI_GRAYSCALE_0(row_src[209],0x00);
		row_dst[210]=ALT_CI_CI_GRAYSCALE_0(row_src[210],0x00);
		row_dst[211]=ALT_CI_CI_GRAYSCALE_0(row_src[211],0x00);
		row_dst[212]=ALT_CI_CI_GRAYSCALE_0(row_src[212],0x00);
		row_dst[213]=ALT_CI_CI_GRAYSCALE_0(row_src[213],0x00);
		row_dst[214]=ALT_CI_CI_GRAYSCALE_0(row_src[214],0x00);
		row_dst[215]=ALT_CI_CI_GRAYSCALE_0(row_src[215],0x00);
		row_dst[216]=ALT_CI_CI_GRAYSCALE_0(row_src[216],0x00);
		row_dst[217]=ALT_CI_CI_GRAYSCALE_0(row_src[217],0x00);
		row_dst[218]=ALT_CI_CI_GRAYSCALE_0(row_src[218],0x00);
		row_dst[219]=ALT_CI_CI_GRAYSCALE_0(row_src[219],0x00);
		row_dst[220]=ALT_CI_CI_GRAYSCALE_0(row_src[220],0x00);
		row_dst[221]=ALT_CI_CI_GRAYSCALE_0(row_src[221],0x00);
		row_dst[222]=ALT_CI_CI_GRAYSCALE_0(row_src[222],0x00);
		row_dst[223]=ALT_CI_CI_GRAYSCALE_0(row_src[223],0x00);
		row_dst[224]=ALT_CI_CI_GRAYSCALE_0(row_src[224],0x00);
		row_dst[225]=ALT_CI_CI_GRAYSCALE_0(row_src[225],0x00);
		row_dst[226]=ALT_CI_CI_GRAYSCALE_0(row_src[226],0x00);
		row_dst[227]=ALT_CI_CI_GRAYSCALE_0(row_src[227],0x00);
		row_dst[228]=ALT_CI_CI_GRAYSCALE_0(row_src[228],0x00);
		row_dst[229]=ALT_CI_CI_GRAYSCALE_0(row_src[229],0x00);
		row_dst[230]=ALT_CI_CI_GRAYSCALE_0(row_src[230],0x00);
		row_dst[231]=ALT_CI_CI_GRAYSCALE_0(row_src[231],0x00);
		row_dst[232]=ALT_CI_CI_GRAYSCALE_0(row_src[232],0x00);
		row_dst[233]=ALT_CI_CI_GRAYSCALE_0(row_src[233],0x00);
		row_dst[234]=ALT_CI_CI_GRAYSCALE_0(row_src[234],0x00);
		row_dst[235]=ALT_CI_CI_GRAYSCALE_0(row_src[235],0x00);
		row_dst[236]=ALT_CI_CI_GRAYSCALE_0(row_src[236],0x00);
		row_dst[237]=ALT_CI_CI_GRAYSCALE_0(row_src[237],0x00);
		row_dst[238]=ALT_CI_CI_GRAYSCALE_0(row_src[238],0x00);
		row_dst[239]=ALT_CI_CI_GRAYSCALE_0(row_src[239],0x00);

		/*for (int x = 0; x < w_subimg; x += 40) {

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

		    row_dst[32] = ALT_CI_CI_GRAYSCALE_0(row_src[32], 0x00);
		    row_dst[33] = ALT_CI_CI_GRAYSCALE_0(row_src[33], 0x00);
		    row_dst[34] = ALT_CI_CI_GRAYSCALE_0(row_src[34], 0x00);
		    row_dst[35] = ALT_CI_CI_GRAYSCALE_0(row_src[35], 0x00);

		    row_dst[36] = ALT_CI_CI_GRAYSCALE_0(row_src[36], 0x00);
		    row_dst[37] = ALT_CI_CI_GRAYSCALE_0(row_src[37], 0x00);
		    row_dst[38] = ALT_CI_CI_GRAYSCALE_0(row_src[38], 0x00);
		    row_dst[39] = ALT_CI_CI_GRAYSCALE_0(row_src[39], 0x00);

		    //On avance de 40*size(short) sur row_src et 40*size(char) sur row_dst
		    row_dst += 40;
		    row_src += 40;
		}*/

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


