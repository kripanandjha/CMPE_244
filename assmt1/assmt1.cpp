/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 */

#include <stdio.h>
#include "utilities.h"
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "io.hpp"
#include "gpio.hpp"
#include "tasks.hpp"
#include "examples/examples.hpp"

#include "LPC17xx.h"

void led_on(void *p)
{
        while(1)
        {

            //if any of the switch is pressed,  turn on the led
            if(((LPC_GPIO2->FIOPIN) & (0x1 << 6)))
               {
                LPC_GPIO2->FIOPIN |= (0x1 << 7);
               }
            else
            {
                LPC_GPIO2->FIOPIN &= ~(0x1 << 7);
            }
            delay_ms(100);

        }
}


int main(void)
{

   // set led to  o/p dir and sw to i/p dir

   // for ext. switch and led
  // BIT(LPC_PINCON->PINMODE4).b15_14 = 0x0; // this is default set no need to set
   LPC_PINCON->PINMODE4  |= (0x3 << 12); // pull down resistor for switch

   LPC_GPIO2->FIODIR |= (0x1 << 7); //set o/p pin
   LPC_GPIO2->FIODIR &= ~(0x1 << 6); //set i/p pin

   // turning off the led
   //LPC_GPIO2->FIOPIN &= ~(0x1 << 7); // by default turning off led
   LPC_GPIO2->FIOPIN |= (0x1 << 7); // by default turning off led


scheduler_add_task(new terminalTask(PRIORITY_MEDIUM));

xTaskCreate(led_on, "led_task", 1024, NULL, PRIORITY_MEDIUM, NULL);
scheduler_start();

}
