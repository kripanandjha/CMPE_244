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
SemaphoreHandle_t gButtonPressSemaphore;

extern "C" {
void (*switch_operation) (void);
void eint3_fun(int,int, void (*fun) (void));
void set_callback(void (*fun1) (void));


void port_0_function(void)
{
    // task specific to port 0
    printf("in port 0 interrupt raised\n");
}
void port_2_function(void)
{
    // task specific to port 2
    printf("In port 2 interrupt raised\n");

}

void (* const callback_fun[])(void) =
{
        port_0_function,
        0,
        port_2_function,
};


void button_pressed(void* p)
{
    while(1)
    {
        if (xSemaphoreTake(gButtonPressSemaphore, portMAX_DELAY)) {
            /* Process the interrupt */
            switch_operation();
            vTaskDelay(1000);
        }
    }
}
}
/* Declare and create the semaphore in main() */
int main(void)
{
    vSemaphoreCreateBinary(gButtonPressSemaphore);
    int gpio_p, pin_n;
    printf("enter gpio port number (0,2) and pin number\n");
    printf("gpio port 0 supported pin (0, 1, 26, 29, 30)\n");
    printf("gpio port 2 supported pin (0-9)\n");
    scanf("%d%d", &gpio_p, &pin_n);

    // port 0, 1, 26, 29, 30
    // port 2 0 - 9

    printf("%d %d\n", gpio_p, pin_n);

    // enabling interrupt for raising edge only
    eint3_fun(gpio_p, pin_n, callback_fun[gpio_p]); // enabling eint3 fun

    xTaskCreate(button_pressed, "switch_task", 1024, NULL, PRIORITY_MEDIUM, NULL);
    vTaskStartScheduler();
    return -1;
}
