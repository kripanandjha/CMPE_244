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
#include <math.h>
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

typedef enum {invalid, up, down, left, right}ACCEL_DATA;

QueueHandle_t q1  = 0;
void task_1(void *p) // task that polls acceleration sensor and sends the data to the queue
{
    ACCEL_DATA data = invalid;
    uint16_t zz_last = 0;
    uint16_t yy_last = 0;
    while(1)
    {
        // gets the acceleration data from the accel. sensor
        uint16_t yy = AS.getX();
        uint16_t zz = AS.getZ();
        //printf("x %4i y %4i z %4i\n",xx, yy, zz);

        if(yy > yy_last+5)
              data = up;
          else if (yy < yy_last)
              data = down;

        if(zz > zz_last+5)
            data = up;
        else if (zz < zz_last-10)
            data = down;

        puts("sen");
        // send the data to the queue.
        if(!xQueueSend(q1, &data, 0))
        {
            printf("failed to send the data in the queue\n");
        }
        puts("sent");
        zz_last = zz;
        yy_last = yy;
        vTaskDelay(1000);
    }
}
void task_2(void *p) // receive the queue data sent by task_1
{
    ACCEL_DATA data;
    while(1)
    {
        //puts("get");
        if(!xQueueReceive(q1, &data, portMAX_DELAY))
        {
            printf("control never comes here\n");
        }
        puts("got");
        switch(data)
        {
            case up:
                printf("up\n");
                break;
            case down:
                printf("down\n");
                break;
            case left:
                printf("left\n");
                break;
            case right:
                printf("right\n");
                break;
            default:
                printf("invalid data\n");
        }
    }
}

int main()
{
    q1 = xQueueCreate(1, sizeof(int));
    xTaskCreate(task_1, "task_1", 1024, NULL, PRIORITY_MEDIUM, NULL);
    xTaskCreate(task_2, "task_2", 1024, NULL, PRIORITY_HIGH, NULL);
    //xTaskCreate(task_2, "task_2", 1024, NULL, PRIORITY_MEDIUM, NULL);
    vTaskStartScheduler();
    return -1;
}