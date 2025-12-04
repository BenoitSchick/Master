#include <stdio.h>
#include <system.h>
#include <stdlib.h>
#include <io.h>
#include "lcd_simple.h"
#include "grayscale.h"
#include "i2c.h"
#include "camera.h"
#include "vga.h"
#include "dipswitch.h"
#include "sobel.h"
#include "sys/alt_timestamp.h"
#include "alt_types.h"

#include <sys/alt_cache.h>

alt_u32 start_sobel_x = 0;
alt_u32 end_sobel_x = 0;

alt_u32 start_sobel_y = 0;
alt_u32 end_sobel_y = 0;

alt_u32 start_sobel_complete = 0;
alt_u32 end_sobel_complete = 0;

alt_u32 start_conv_grayscale = 0;
alt_u32 end_conv_grayscale = 0;

alt_u32 start_sobel_threshold = 0;
alt_u32 end_sobel_threshold = 0;

const int H_SUBIMG = 128; //nbre de pixels height de la subimage
const int W_SUBIMG = 128; //nbre de pixels width de la subimage

int main()
{
  void *buffer1,*buffer2,*buffer3,*buffer4;
  unsigned short *image;
  unsigned char *grayscale;
  unsigned char current_mode;
  unsigned char mode;
  init_LCD();
  init_camera();
  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
  printf("Hello from Nios II!\n");
  cam_get_profiling();
  buffer1 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer2 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer3 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer4 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  cam_set_image_pointer(0,buffer1);
  cam_set_image_pointer(1,buffer2);
  cam_set_image_pointer(2,buffer3);
  cam_set_image_pointer(3,buffer4);
  enable_continues_mode();
  //build_LUT_rgb_to_gray(cam_get_xsize()>>1,cam_get_ysize()); // construit une LUT pour optimiser la conversion rgb to grayscale
  	  	  	  	  	  	   // qui se fera hors de la boucle infini --> gain de performances



  //Declaration for inline function sobel_x & sobel_y
  int width = (cam_get_xsize()>>1);
  int height = cam_get_ysize();

  init_grayscale_arrays(cam_get_xsize()>>1,cam_get_ysize());
  init_sobel_arrays(cam_get_xsize()>>1,cam_get_ysize());
  //init_grayscale_arrays(width, height);
  //init_sobel_arrays(width, height);

  do {
	  alt_timestamp_start();
	  if (new_image_available() != 0) {
		  if (current_image_valid()!=0) {
			  current_mode = DIPSW_get_value();
			  mode = current_mode&(DIPSW_SW1_MASK|DIPSW_SW3_MASK|DIPSW_SW2_MASK);
			  image = (unsigned short*)current_image_pointer();
		      switch (mode) {
		      case 0 : transfer_LCD_with_dma(&image[16520],
		                	cam_get_xsize()>>1,
		                	cam_get_ysize(),0);
		      	  	  printf("mode 0 \n");
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }

		      	  	   break;
		      case 1 :
		    	  printf("mode 1 \n");
		    	  /*conv_grayscale((void *)image,		//convertit l'image en niveaux de gris
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());*/
		    	  for (int y = 0; y < height; y += H_SUBIMG) {
		    		  for (int x = 0; x < width; x += W_SUBIMG) {
		    			  conv_grayscale_with_subimg( image, x, y, W_SUBIMG, H_SUBIMG, width, height);
		    		  }
		    	  }
		    	  //conv_grayscale_with_subimg(image, 0, 0, 240, 320, width, height);

				   grayscale = get_grayscale_picture();	//rï¿½cupï¿½re l'image en niveau de gris

				   transfer_LCD_with_dma(&grayscale[16520],	//affiche l'image en niveau de gris sur le LCD
				   									cam_get_xsize()>>1,
				   									cam_get_ysize(),1);
				   /*transfer_LCD_with_dma(&grayscale[0],	//affiche l'image en niveau de gris sur le LCD
									cam_get_xsize()>>1,
									cam_get_ysize(),1);*/

				   if ((current_mode&DIPSW_SW8_MASK)!=0) {
					  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
					  vga_set_pointer(grayscale);
				   }

				   break;
		      case 2 :
		    	  start_conv_grayscale = alt_timestamp();
		    	  conv_grayscale((void *)image,
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		    	  end_conv_grayscale = alt_timestamp();
		      	  	  printf("mode 2 \n");

		               grayscale = get_grayscale_picture();


		               start_sobel_x = alt_timestamp();
		               sobel_x_with_rgb(grayscale);
		               end_sobel_x = alt_timestamp();

		               image = GetSobel_rgb();
		               transfer_LCD_with_dma(&image[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }

		      	  	   break;
		      case 3 :

		    	  start_conv_grayscale = alt_timestamp();
		    	  conv_grayscale((void *)image,					//convertit l'image en niveaux de gris
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		    	  end_conv_grayscale = alt_timestamp();
		      printf("mode 3 \n");

		      	  	   grayscale = get_grayscale_picture();		//rï¿½cupï¿½re l'image en niveau de gris


		      	  	   start_sobel_x = alt_timestamp();			//applique filtre de sobel sur image en niveaux de gris
		               sobel_x(grayscale);
		               end_sobel_x = alt_timestamp();

		               start_sobel_y = alt_timestamp();
		               sobel_y_with_rgb(grayscale);
		               end_sobel_y = alt_timestamp();


		               image = GetSobel_rgb();				  //recupï¿½re image filtrï¿½e avec filtre de sobel
		               /*transfer_LCD_with_dma(&image[16520],  //affiche l'image filtrï¿½e sur le LCD
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),0);*/
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      default:
		    	  printf("default mode\n");

		    	  //Camera donne une image : 512x384   (width x height)
		    	  //LCD affiche une image :  240x320   (width x height)

		    	  //L'idée est de traiter une sous-image 320x240 de l'image de la camera

		    	  //--> row = picture + y*width(512)  avec y de 0 à 320 --> on parcourt 320 lignes au lieu de 384
		    	  //--> row[x] avec x de 0 à 240  --> on parcourt 240pixels/ligne au lieu de 520


		    	  start_conv_grayscale = alt_timestamp();

		    	  //grayscale sur une image 320x240 --> taille du LCD
		    	  conv_grayscale_with_subimg(image, 0, 0, 240, 320, width, height);

		    	  end_conv_grayscale = alt_timestamp();

		    	  printf("grayscale_done\n");
		    	  grayscale = get_grayscale_picture();			//recupere l'image en niveau de gris

		    	  printf("grayscale_picture return\n");
		    	  start_sobel_complete = alt_timestamp();

		    	  //filtre de sobel sur une image 320x240 --> taille du LCD
		    	  sobel_complete_with_subimg(grayscale, 128, 0, 0, 240, 320);

		    	  end_sobel_complete = alt_timestamp();

		    	  printf("sobel done\n");
		    	  grayscale=GetSobelResult();

		    	  transfer_LCD_with_dma(&grayscale[0], 		//affiche l'image filtre sur le LCD
		    	  		    			  	  	  	  	cam_get_xsize()>>1,
		    	  										cam_get_ysize(),1);


		    	  /*conv_grayscale((void *)image,					//convertit l'image en niveaux de gris
		    	  		    	  		    	  	                                  cam_get_xsize()>>1,
		    	  		    	  		    	  	                                  cam_get_ysize());*/


				  /*for (int y = 0; y < height; y += H_SUBIMG) {
					  for (int x = 0; x < width; x += W_SUBIMG) {
						  conv_grayscale_with_subimg(image, x, y, W_SUBIMG, H_SUBIMG, width, height);
					  }
				  }*/


				  //sobel_complete(grayscale, 128);				//applique le filtre de sobel sur l'image (grayscale)

								  /*for (int y = 0; y < height; y += H_SUBIMG) {
									  for (int x = 0; x < width; x += W_SUBIMG) {
										  sobel_complete_with_subimg(grayscale, 128, x, y, W_SUBIMG, H_SUBIMG);
									  }
					}*/

		    	  /*transfer_LCD_with_dma(&grayscale[16520], 		//affiche l'image filtre sur le LCD
		    			  	  	  	  	cam_get_xsize()>>1,
										cam_get_ysize(),1);*/


		    	  if ((current_mode&DIPSW_SW8_MASK)!=0) {
		    		  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
		    		  vga_set_pointer(grayscale);
		    	  }
		    	  break;
		      }

		      printf("nbre_cycles sobel complete : %lu\n", (end_sobel_complete - start_sobel_complete));
		  	  //printf("nbre_cycles sobel X : %lu\n", (end_sobel_x - start_sobel_x));
		      //printf("nbre_cycles sobel Y : %lu\n", (end_sobel_y - start_sobel_y));
		      //printf("nbre_cycles sobel threshold : %lu\n", (end_sobel_threshold - start_sobel_threshold));
		      printf("nbre_cycles conv greyscale : %lu\n", (end_conv_grayscale - start_conv_grayscale));
		  }
	  }



  } while (1);
  return 0;
}
