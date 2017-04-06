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
#include "eint.h"
#include "semphr.h"

#include "LPC17xx.h"

//typedef QueueHandle_t SemaphoreHandle_t;
// semaphore handle and queue handle both are of same type
SemaphoreHandle_t sem = 0;

// we can use parameter p to send semaphore var.
SoftTimer t1(200);
void switch_func(void* p)
{
    while(1)
    {
        if (xSemaphoreTake(sem, portMAX_DELAY)) {
                printf("switch pressed\n");
        }
    }
}

void isr_func(void)
{
    long yield = 0;
    // give semaphore to the switch_function
    if(xSemaphoreGiveFromISR(sem, &yield))

    {
        t1.reset(200);
        /* Process the interrupt */
        while(!t1.expired());
        // yielding interrupt service routine
        portYIELD_FROM_ISR(yield);
    }
}

int main(void)
{
                     //xQueueCreate(size_of_queue, size_of_each_item_of_the_queue);
//    qh = xQueueCreate(1, sizeof(int));
    sem = xSemaphoreCreateBinary(); 
    // gpio port 2 pin 7 as input pin
   // LPC_PINCON->PINSEL4 &= ~(0x3 << 14 ); //setting it zero to work as a gpio pin
    
    // no need to set pinmode by default set it to pull up mode
    //LPC_GPIO2->FIODIR0 &= ~(0x1 << 7); // setting as a zero to work as a input pin
  
    //xTaskCreate(func_address, "task_name", stack_frame_size, passring_param_address, task_priority, task_call_back_function_handler);
    xTaskCreate(switch_func, "task_tx", 1024, NULL, PRIORITY_MEDIUM, NULL);
    
    eint3_enable_port2(7, eint_rising_edge, isr_func);

    vTaskStartScheduler(); // use this call when adding any fuction as a task

    // control never comes down never
    return -1;

}
