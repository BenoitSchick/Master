#include "io.h"
#include <stdio.h>
#include "system.h"
#include <stdint.h>
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"


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




static uint32_t counter;

void timer_interrupt(void *context, alt_u32 id)
{
	counter++;

	// write counter value on the parallel port
	IOWR_32DIRECT(PARALLELPORT_0_BASE,2*4,counter); // -->regport

	//clear pending interrupt
	IOWR_16DIRECT(TIMER_0_BASE, ALTERA_AVALON_TIMER_STATUS_REG*4, 0x00);
}


int main()
{
	//uint32_t dir, port;
	counter = 0;

	printf("Lets start counting \n");


	// Configurer les 32 bits du port en sortie
	IOWR_32DIRECT(PARALLELPORT_0_BASE, 0x0, 0xFFFFFFFF);  // RegDir

	//stop timer
	IOWR_16DIRECT(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_REG*4, 0x04);

	//clear pending interrupt
	IOWR_16DIRECT(TIMER_0_BASE, ALTERA_AVALON_TIMER_STATUS_REG*4, 0x00);


	//start timer (bit0 = interrupt enable, bit1 = continous count, bit2 = start)
	IOWR_16DIRECT(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_REG*4, 0x07);


	//Register ISR function
	//alt_irq_register(TIMER_0_IRQ, NULL, timer_interrupt);
	if (alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, (alt_isr_func)timer_interrupt, NULL, NULL) != 0)
	{
	        return -1;
	}


	while(1)
	{
		//port = IORD_32DIRECT(PARALLELPORT_0_BASE, 2);
		//printf("Port register = 0x%08X\n", port);
	}
}
