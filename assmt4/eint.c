#include <stdio.h>
#include "FreeRTOS.h"
#include "eint.h"
#include "LPC17xx.h"
#include "semphr.h"


/* put dummy function for wireless function support*/
void eint3_enable_port0(uint8_t pin_num, eint_intr_t type, void_func_t func)
{
}
/*
 * Setting EINT3 interrupt for the port 0 and port 2
 * step I ==>
 * enable EINT3 interrupt in GPIO port 2 pin 13
 * select PINSEL4 port 2 pin 13 pin to value 1 to enable EINT3
 * PINMODE is default set to i/p mode so no need to set explicitly 
 * Also enable NVIC EINT3 interrupt, a unique no. is assigned to this int. in nvic
 * step II ==>
 * enable port 0 and port 2 rising edge or falling based on your requirement
 * Right now I am just enabling rising edge interrupt for the port 0 and port 2
 * IO0IntEnR enable port 0 rising edge, set all pins to 1 except 12:14, 31
 * IO2IntEnR enable port 2 rising edge, set all pins except 31:14
 * IOIntStatus - 0x4002 8080
 * bit 0 - port 0 - interrupt pending
 * bit 2 - port 2 - interrupt pending
 * step III
 * when interrupt raised and control comes into your isr, find out
 * the interrupt source and first clear the interrupt source
 * use IO0IntClr to clear port 0 interrupt source
 * and use IO2IntClr to clear port 2 interrupt source
 * step IV
 * Use first IOIntStatus to find out interrupt is raised from port 0 or port 2
 * if bit 0 is set interrupt is raised from port 0
 * if bit 2 is set interrupt is raised from port 2
 * based on the bit 0 or bit 2 set design the if condition
 * one should work for port 0 and another should work for port 2
 * further in port x condition find out the gpio source by
 * IOxIntStatR which gpio pin raised the interrupt
 * IO0IntStatR for port 0, Reserved 14:12 and 31 ping
 * IO2IntStatR for port 2, Reserved 31:14 pins
 */
extern SemaphoreHandle_t gButtonPressSemaphore;
extern void (*switch_operation) (void);

void set_callback(void (*fun1) (void))
{
    if(fun1 != NULL)
        switch_operation = fun1;
    else
        printf("Invalid function passed\n");
}
//you can add your own isr function
void EINT3_IRQHandler()
{
    uint32_t t1;
    // step III
    // first find out interrupt source
    if(LPC_GPIOINT->IntStatus & 1) // interrupt source GPIO port 0
    {
        // clear interrupt source
         t1 = LPC_GPIOINT->IO0IntStatR;
        LPC_GPIOINT->IO0IntClr = t1;
    }
    else if(LPC_GPIOINT->IntStatus & (1 << 2)) // interrupt source GPIO port 2
    {
        // clear interrupt source
         t1 = LPC_GPIOINT->IO2IntStatR;
        LPC_GPIOINT->IO2IntClr = t1;

    }
    xSemaphoreGiveFromISR(gButtonPressSemaphore, NULL);
        portYIELD_FROM_ISR(0);

}
void eint3_fun(int gpio_p, int pin_n, void (*fun) (void))
{
	
	// no need to set pinmode by default set it to pull up mode
	// also set all port 0 and port 2 pins to i/p port to trigger interrupt eint3
    // but no need to set as i/p by default they are set as i/p pin

    // No need to set FIOxDIR pin also as by default set it as i/p pin only

    // step 1
	// enable eint3 interrupt, which is port 2 pin 13
	LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(0x3 << 26)) | (0x1 << 26);
    set_callback(fun);
	// step 2
	if(gpio_p == 0) {
	    LPC_GPIOINT->IO0IntEnR = 0x1 << pin_n; // enable all gpio port 0 pins
	}
	if(gpio_p == 2) {
	    LPC_GPIOINT->IO2IntEnR = 0x1 << pin_n; // enable all gpio port 2 pins
	}
	/* Enable NVIC EINT3 pin*/
	NVIC_EnableIRQ(EINT3_IRQn); // bit 21
}
