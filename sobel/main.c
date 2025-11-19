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
  init_sobel_arrays(cam_get_xsize()>>1,cam_get_ysize());

  alt_timestamp_start();

  //Declaration for inline function sobel_x & sobel_y
  int width = cam_get_xsize()>>1;
  int height = cam_get_ysize();

  /*static short *sobel_x_result;
  static short *sobel_y_result;
  sobel_x_result = (short *)malloc(width*height*sizeof(short));
  sobel_y_result = (short *)malloc(width*height*sizeof(short));

  for (int loop = 0 ; loop < width*height ; loop++) {
  	sobel_x_result[loop] = 0;
  	sobel_y_result[loop] = 0;
  }*/

  //const char gx_array[9] = {-1,0,1,-2,0,2,-1,0,1};
  //const char gy_array[9] = {1,2,1,0,0,0,-1,-2,-1};

  /*const char gx_array[3][3] = {{-1,0,1},
                               {-2,0,2},
                               {-1,0,1}};

  const char gy_array[3][3] = { {1, 2, 1},
                                {0, 0, 0},
                               {-1,-2,-1}};*/

  do {
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
		      case 1 : conv_grayscale((void *)image,		//convertit l'image en niveaux de gris
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		      	  	  printf("mode 1 \n");
		               grayscale = get_grayscale_picture();	//r�cup�re l'image en niveau de gris

		               transfer_LCD_with_dma(&grayscale[16520],	//affiche l'image en niveau de gris sur le LCD
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),1);
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

		      	  	   grayscale = get_grayscale_picture();		//r�cup�re l'image en niveau de gris


		      	  	   start_sobel_x = alt_timestamp();			//applique filtre de sobel sur image en niveaux de gris
		               sobel_x(grayscale);
		               end_sobel_x = alt_timestamp();

		               start_sobel_y = alt_timestamp();
		               sobel_y_with_rgb(grayscale);
		               end_sobel_y = alt_timestamp();


		               image = GetSobel_rgb();				  //recup�re image filtr�e avec filtre de sobel
		               transfer_LCD_with_dma(&image[16520],  //affiche l'image filtr�e sur le LCD
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      default:
		    	  printf("default mode\n");


		    	  start_conv_grayscale = alt_timestamp();
		    	  conv_grayscale((void *)image,					//convertit l'image en niveaux de gris
	                                  cam_get_xsize()>>1,
	                                  cam_get_ysize());
		    	  end_conv_grayscale = alt_timestamp();

		    	  grayscale = get_grayscale_picture();			//r�cup�re l'image en niveau de gris


                      //start_sobel_x = alt_timestamp();			//applique filtre de sobel sur image en niveaux de gris
                      //sobel_x(grayscale);


                     //INLINE SOBEL X
                      /*int x,y;
					  for (y = 1 ; y < (height-1) ; y++) {
						 for (x = 1 ; x < (width-1) ; x++) {

						   short result = 0;

						   result += gx_array[(0)*3+(0)] * grayscale[(y-1)*width+(x-1)];
						   result += gx_array[(0)*3+(1)] * grayscale[(y-1)*width+(x+0)];
						   result += gx_array[(0)*3+(2)] * grayscale[(y-1)*width+(x+1)];


						   result += gx_array[(1)*3+(0)] * grayscale[(y)*width+(x-1)];
						   result += gx_array[(1)*3+(1)] * grayscale[(y)*width+(x+0)];
						   result += gx_array[(1)*3+(2)] * grayscale[(y)*width+(x+1)];


						   result += gx_array[(2)*3+(0)] * grayscale[(y+1)*width+(x-1)];
						   result += gx_array[(2)*3+(1)] * grayscale[(y+1)*width+(x+0)];
						   result += gx_array[(2)*3+(2)] * grayscale[(y+1)*width+(x+1)];

						   sobel_x_result[y*width+x] = result;
							//sobel_x_result[y*width+x] = sobel_mac(grayscale,x,y,gx_array,width);
						 }
					  }*/

                       //end_sobel_x = alt_timestamp();


                       //start_sobel_y = alt_timestamp();
                       //sobel_y(grayscale);

                      //INLINE SOBEL Y
                      /*for (y = 1 ; y < (height-1) ; y++) {
                             for (x = 1 ; x < (width-1) ; x++) {

                            	 short result = 0;


							   result += gy_array[(0)*3+(0)] * grayscale[(y-1)*width+(x-1)];
							   result += gy_array[(0)*3+(1)] * grayscale[(y-1)*width+(x+0)];
							   result += gy_array[(0)*3+(2)] * grayscale[(y-1)*width+(x+1)];


							   result += gy_array[(1)*3+(0)] * grayscale[(y)*width+(x-1)];
							   result += gy_array[(1)*3+(1)] * grayscale[(y)*width+(x+0)];
							   result += gy_array[(1)*3+(2)] * grayscale[(y)*width+(x+1)];


							   result += gy_array[(2)*3+(0)] * grayscale[(y+1)*width+(x-1)];
							   result += gy_array[(2)*3+(1)] * grayscale[(y+1)*width+(x+0)];
							   result += gy_array[(2)*3+(2)] * grayscale[(y+1)*width+(x+1)];

                                sobel_y_result[y*width+x] = result;
                                //sobel_y_result[y*width+x] = sobel_mac(grayscale,x,y,gy_array,width);
                             }
                          }*/
                       //end_sobel_y = alt_timestamp();


                       //start_sobel_threshold = alt_timestamp();
                       //sobel_threshold(128);
                       //end_sobel_threshold = alt_timestamp();

                       start_sobel_complete = alt_timestamp();
                       sobel_complete(grayscale, 128);
                       end_sobel_complete = alt_timestamp();


                       grayscale=GetSobelResult();				//applique le filtre de sobel sur l'image (grayscale)
		               transfer_LCD_with_dma(&grayscale[16520], //affiche l'image filtr�e sur le LCD
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),1);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
		      	  		  vga_set_pointer(grayscale);
		      	  	   }
		      	  	   break;
		      }

		      printf("nbre_cycles sobel complete : %lu\n", (end_sobel_complete - start_sobel_complete));
		  	  //printf("nbre_cycles sobel X : %lu\n", (end_sobel_x - start_sobel_x));
		      //printf("nbre_cycles sobel Y : %lu\n", (end_sobel_y - start_sobel_y));
		      printf("nbre_cycles sobel threshold : %lu\n", (end_sobel_threshold - start_sobel_threshold));
		      printf("nbre_cycles conv greyscale : %lu\n", (end_conv_grayscale - start_conv_grayscale));
		  }
	  }



  } while (1);
  return 0;
}
