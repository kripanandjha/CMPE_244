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
#include <string.h>
#include "fat/disk/spi_flash.h"
#include "utilities.h"
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "io.hpp"
#include "gpio.hpp"
#include "tasks.hpp"
#include "vector.hpp"
#include "examples/examples.hpp"

#include "printf_lib.h"

#include "LPC17xx.h"
#include "bio.h"
#include "semphr.h"
#include "uart0_min.h"

void fun1(void* p)
{
    //TickType_t xLastWakeTime = xTaskGetTickCount();
    //const TickType_t xFrequency = 1000;

    while(1) {
        uart0_puts("aaaaaaaaaaaaaaaaaaaaaa");

        //vTaskDelayUntil(&xLastWakeTime, xFrequency);
        vTaskDelay(1000);
    }
}
void fun2(void* p)
{
    //TickType_t xLastWakeTime = xTaskGetTickCount();
    //const TickType_t xFrequency = 1000;
    while(1) {
        uart0_puts("bbbbbbbbbbbbbbbb");
        
       // vTaskDelayUntil(&xLastWakeTime, xFrequency);
        vTaskDelay(1000);
    }
}



int main(void)
{

    xTaskCreate(fun1, "fun1_task", 1024, NULL, PRIORITY_HIGH, NULL);
    xTaskCreate(fun2, "fun2_task", 1024, NULL, PRIORITY_MEDIUM, NULL);

    vTaskStartScheduler();
    return -1;
}
