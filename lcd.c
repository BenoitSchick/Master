#include "io.h"
#include <stdio.h>
#include <stdint.h>
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "system.h"
#include "image_DMA.h"


#define LCD_WIDTH  240
#define LCD_HEIGHT 320

#define LCD_RESET_n 0x01
#define LCD_IM0 0x04

#define IMAGE_SIZE (2*240*320) // 2 bytes, 240x320

#define GPIO_REG_CS (4*6)

enum DMA_LCD_REG {
	DMA_LCD_COMMANDE = 0,
	DMA_LCD_DATA = 4,
	DMA_LCD_PTR = 8,
	DMA_LCD_SIZE = 12,
	DMA_LCD_CTRL = 16,
};

enum GPIO_REG {
	GPIO_REG_DIR = 0,
	GPIO_REG_PORT = 8,
	GPIO_REG_SET = 12,
	GPIO_REG_CLR = 16,
};

enum GPIO_REG_LCD {
	GPIO_REG_DIR_LCD = 0,
	GPIO_REG_PORT_LCD = 2,
	GPIO_REG_SET_LCD = 3,
	GPIO_REG_CLR_LCD = 4,
};



///////////////////
// Theorie Timer //
///////////////////

/*
 * Quand timer generer une interruption, il met un bit de statut IRQ pending à 1
 * Quand le controleur d'interruption de Nios II voit ce bit, il appelle ISR
 * ISR doit effacer ce bit pour dire qu'il a traité l'interruption (acknowledge)
*/

/*
 * Le compteur décompte d'une valeur à 0. Dès qu'il atteint 0, il set à 1 le bit0 (TO) du status register (à clear nous meme).
 * Qunad le bit0 du status register est actif, et le bit0 du control register est actif ==> interval timer genere une IRQ
 *
 * --> bit0 du control register => active les interruptions du timer & lance une interruption quand compteur atteint 0
 * --> bit0 du status register => s'active lorsque le compteur atteint 0
 *
 * */

static int calcul_DMA_perf = 1;
static int img_transfered = 1;
static int img_select = 0;
static uint32_t counter;
static uint32_t counter_transferDMA_img = 0;
static uint32_t counter_transferCPU_img = 0;

void delay(uint32_t ticks) {
    counter = 0;
    while (counter < ticks);
}

void timer_perf_interrupt(void *context)
{
	counter_transferDMA_img++;
	counter_transferCPU_img++;
	//clear pending interrupt
	IOWR_16DIRECT(TIMER_1_BASE, 0, 0);
}

void timer_interrupt(void *context)
{
	counter++;
	// write counter value on the parallel port
	IOWR_32DIRECT(PARALLELPORT_0_BASE,GPIO_REG_PORT,counter);

	//clear pending interrupt
	IOWR_16DIRECT(TIMER_0_BASE, 0, 0);
}

void dma_interrupt(void *context)
{
	// Clear IRQ flag by writing 0x04 to the control register
	IOWR_32DIRECT(DMA_LCD_CTRL_0_BASE, DMA_LCD_CTRL, 0x0004);
	img_transfered = 1;

	if(calcul_DMA_perf == 1) {
		printf("Temps de tranfert de la DMA : %d ms\n", counter_transferDMA_img);
		calcul_DMA_perf = 0;
	}
	//printf("acknowledge done \n");
}

void LCD_Write_Command(int command) {
    IOWR_16DIRECT(DMA_LCD_CTRL_0_BASE,DMA_LCD_COMMANDE,command);
}

void LCD_Write_Data(int data) {
    IOWR_16DIRECT(DMA_LCD_CTRL_0_BASE,DMA_LCD_DATA,data);
}


void init_LCD() {

	//IOWR_8DIRECT(GPIO_LCD_0_BASE, GPIO_REG_SET_LCD, LCD_RDn);
	IOWR_32DIRECT(DMA_LCD_CTRL_0_BASE,GPIO_REG_CS, 1); // cs_n off
	IOWR_8DIRECT(GPIO_LCD_0_BASE, GPIO_REG_CLR_LCD, LCD_RESET_n|LCD_IM0);
	delay(200);
	//IOWR_8DIRECT(GPIO_LCD_0_BASE, GPIO_REG_CLR_LCD, LCD_CS_n); // set reset off and 16 bits mode and enable LED_CS
	IOWR_8DIRECT(GPIO_LCD_0_BASE, GPIO_REG_SET_LCD, LCD_RESET_n);
	IOWR_32DIRECT(DMA_LCD_CTRL_0_BASE,GPIO_REG_CS, 0); // cs_n on
	delay(200);

	LCD_Write_Command(0x0028);     //display OFF
	LCD_Write_Command(0x0011);     //exit SLEEP mode
	LCD_Write_Data(0x0000);

	LCD_Write_Command(0x00CB);     //Power Control A
	LCD_Write_Data(0x0039);     //always 0x39
	LCD_Write_Data(0x002C);     //always 0x2C
	LCD_Write_Data(0x0000);     //always 0x00
	LCD_Write_Data(0x0034);     //Vcore = 1.6V
	LCD_Write_Data(0x0002);     //DDVDH = 5.6V

	LCD_Write_Command(0x00CF);     //Power Control B
	LCD_Write_Data(0x0000);     //always 0x00
	LCD_Write_Data(0x0081);     //PCEQ off
	LCD_Write_Data(0x0030);     //ESD protection

	LCD_Write_Command(0x00E8);     //Driver timing control A
	LCD_Write_Data(0x0085);     //non - overlap
	LCD_Write_Data(0x0001);     //EQ timing
	LCD_Write_Data(0x0079);     //Pre-charge timing


	LCD_Write_Command(0x00EA);     //Driver timing control B
	LCD_Write_Data(0x0000);        //Gate driver timing
	LCD_Write_Data(0x0000);        //always 0x00

	LCD_Write_Command(0x00ED); //Power‐On sequence control
	LCD_Write_Data(0x0064);        //soft start
	LCD_Write_Data(0x0003);        //power on sequence
	LCD_Write_Data(0x0012);        //power on sequence
	LCD_Write_Data(0x0081);        //DDVDH enhance on

	LCD_Write_Command(0x00F7);     //Pump ratio control
	LCD_Write_Data(0x0020);     //DDVDH=2xVCI

	LCD_Write_Command(0x00C0);    //power control 1
	LCD_Write_Data(0x0026);
	LCD_Write_Data(0x0004);     //second parameter for ILI9340 (ignored by ILI9341)

	LCD_Write_Command(0x00C1);     //power control 2
	LCD_Write_Data(0x0011);

	LCD_Write_Command(0x00C5);     //VCOM control 1
	LCD_Write_Data(0x0035);
	LCD_Write_Data(0x003E);

	LCD_Write_Command(0x00C7);     //VCOM control 2
	LCD_Write_Data(0x00BE);

	LCD_Write_Command(0x00B1);     //frame rate control
	LCD_Write_Data(0x0000);
	LCD_Write_Data(0x0010);

	LCD_Write_Command(0x003A);    //pixel format = 16 bit per pixel
	LCD_Write_Data(0x0055);

	LCD_Write_Command(0x00B6);     //display function control
	LCD_Write_Data(0x000A);
	LCD_Write_Data(0x00A2);

	LCD_Write_Command(0x00F2);     //3G Gamma control
	LCD_Write_Data(0x0002);         //off

	LCD_Write_Command(0x0026);     //Gamma curve 3
	LCD_Write_Data(0x0001);

	LCD_Write_Command(0x0036);     //memory access control = BGR
	LCD_Write_Data(0x0000);

	LCD_Write_Command(0x002A);     //column address set
	LCD_Write_Data(0x0000);
	LCD_Write_Data(0x0000);        //start 0x0000
	LCD_Write_Data(0x0000);
	LCD_Write_Data(0x00EF);        //end 0x00EF

	LCD_Write_Command(0x002B);    //page address set
	LCD_Write_Data(0x0000);
	LCD_Write_Data(0x0000);        //start 0x0000
	LCD_Write_Data(0x0001);
	LCD_Write_Data(0x003F);        //end 0x013F

	LCD_Write_Command(0x0029);
	printf("Finished the init of the LCD\n");

  }

void write_img_DMA(uint16_t* image)
{
	LCD_Write_Command(0x002C);

	//Ask DMA to read 240x360x16bits in SDRAM
	IOWR_32DIRECT(DMA_LCD_CTRL_0_BASE, DMA_LCD_PTR, image);
	IOWR_32DIRECT(DMA_LCD_CTRL_0_BASE, DMA_LCD_SIZE, LCD_HEIGHT*LCD_WIDTH);

	//Start DMA transfer by writing 0x01 to control register
	IOWR_32DIRECT(DMA_LCD_CTRL_0_BASE, DMA_LCD_CTRL, 0x01);
}

void write_img_CPU(uint16_t* image)
{
	LCD_Write_Command(0x002C);

	for(int i=0; i<(LCD_WIDTH*LCD_HEIGHT); i++)
	{
		LCD_Write_Data(image[i]);
	}
}




int main()
{
	/*
	* INITIALISATION
	*/

	counter = 0;

	// Configurer les 32 bits du port en sortie
	IOWR_32DIRECT(PARALLELPORT_0_BASE, GPIO_REG_DIR, 0xFFFFFFFF);
	IOWR_8DIRECT(GPIO_LCD_0_BASE, GPIO_REG_DIR_LCD, 0xFF);

	//stop timer
	IOWR_16DIRECT(TIMER_0_BASE, 0x4, 0x08);

	//clear pending interrupt
	IOWR_16DIRECT(TIMER_0_BASE, 0, 0x00);


	//Register ISR function
	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, (alt_isr_func)timer_interrupt, NULL, NULL);
	alt_ic_isr_register(TIMER_1_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_1_IRQ, (alt_isr_func)timer_perf_interrupt, NULL, NULL);
	alt_ic_isr_register(DMA_LCD_CTRL_0_IRQ_INTERRUPT_CONTROLLER_ID, DMA_LCD_CTRL_0_IRQ, (alt_isr_func)dma_interrupt, NULL, NULL);

	//start timer (bit0 = interrupt enable, bit1 = continous count, bit2 = start)
	IOWR_16DIRECT(TIMER_0_BASE, 0x4, 0x07);
	init_LCD();

	/*
	 * CALCUL PERFORMANCE
	 */

	//stop timer perf
	IOWR_16DIRECT(TIMER_1_BASE, 0x4, 0x08);
	//clear pending interrupt timer perf
	IOWR_16DIRECT(TIMER_1_BASE, 0, 0x00);


	//start timer
	IOWR_16DIRECT(TIMER_1_BASE, 0x4, 0x07);

	//CPU perf
	counter_transferCPU_img = 0;
	write_img_CPU(image_rick_roll);
	printf("Temps de tranfert du CPU : %d ms\n", counter_transferCPU_img);

	//DMA perf
	counter_transferDMA_img = 0;
	write_img_DMA(image_rick_roll);

	if(calcul_DMA_perf == 0)
	{
		//stop timer perf
		IOWR_16DIRECT(TIMER_1_BASE, 0x4, 0x08);
	}


	/*
	 * ECRITURE IMAGES SUR LCD
	 */

	LCD_Write_Command(0x002C); // indique qu'on va écrire des pixels
	img_select = 0;

	while(1)
	{

		if(img_transfered) {
			delay(100);
			if(img_select == 0)
			{
				write_img_DMA(image_mse);
				img_select = 1;

			} else
			{
				write_img_DMA(image_rick_roll);
				img_select = 0;
			}
			img_transfered = 0;
		}
	}
	//port = IORD_32DIRECT(PARALLELPORT_0_BASE, 2);
	//printf("Port register = 0x%08X\n", port);
}
