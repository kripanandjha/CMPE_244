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
#include "utilities.h"
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "io.hpp"
#include "gpio.hpp"
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "i2c2.hpp"
#include "LPC17xx.h"
#include "printf_lib.h"

uint8_t memory[4]={0};
uint8_t mem1[4] = {0};
uint8_t slave_addr = 0x70;


SemaphoreHandle_t sh_t3;
SemaphoreHandle_t sh_t4;



void light_sensor1(void* p)
{
    while(1)
    {
        if (xSemaphoreTake(sh_t3, portMAX_DELAY)) {
                       // printf("switch pressed\n");
                        printf("Light Sensor Value: %d\n",LS.getRawValue());
                }
    }

    }

void ir_sensor1(void* p)
{
    while(1)
    {
        if (xSemaphoreTake(sh_t4, portMAX_DELAY)) {
            // printf("switch pressed\n");
            printf("IR Sensor Value: %d",IS.getLastIRCode());

        }
    }

}
QueueHandle_t queue;
void led_disp(void* p)
{
    uint8_t r;
    while(1)
    {

        if(!xQueueReceive(queue, &r, portMAX_DELAY))
        {
            u0_dbg_printf("unable to set display to data %d \n",r);

        }
        LD.setNumber(r);
        vTaskDelay(1000);
    }

}


class i2c: public scheduler_task
{
    public:
        i2c(uint8_t priority): scheduler_task("i2c", 2048, priority)
    {}
        bool init(void)
        {
            printf("in i2c init\n");
            I2C2::getInstance().init_slave(slave_addr, memory, sizeof(memory));
            return true;
        }
        bool run(void* p)
        {

            if((!memcmp(memory, mem1, sizeof(memory))))
            {
                //u0_dbg_printf("Memory Not modified\n");
            }
            else
            {
                int msize = sizeof(memory)-1;
                while(msize >= 0)
                {
                    if(memory[0] == 1) {
                       // u0_dbg_printf("Light Sensor Value: %d\n",LS.getRawValue());
                        xSemaphoreGive(sh_t3);
                    }
                    if(memory[1] == 2) {
                        //u0_dbg_printf("IR Sensor Value: %d",IS.getLastIRCode());
                        xSemaphoreGive(sh_t4);
                    }


                    xQueueSend(queue, &memory[msize],portMAX_DELAY);

                    msize--;
                }

            }

            vTaskDelay(1000);

            return true;
        }
        //LD.setNumber();

};


int main(void)
{
    vSemaphoreCreateBinary(sh_t3);
    vSemaphoreCreateBinary(sh_t4);

    queue = xQueueCreate(4, sizeof(char));
    scheduler_add_task(new terminalTask(PRIORITY_LOW));
    scheduler_add_task(new i2c(PRIORITY_LOW));
    //xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask );
    xTaskCreate(light_sensor1, "task1", 1024, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(ir_sensor1, "task2", 1024, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(led_disp, "task3", 1024, NULL, PRIORITY_LOW, NULL);
    //xTaskCreate(task_2, "task_2", 1024, NULL, PRIORITY_MEDIUM, NULL);

    scheduler_start();


    return -1;

}
