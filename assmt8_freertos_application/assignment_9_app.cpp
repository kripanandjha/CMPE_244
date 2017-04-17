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

#include <string.h>
#include "fat/disk/spi_flash.h"
#include "vector.hpp"
#include "printf_lib.h"
#include "examples/examples.hpp"

#include "projdefs.h"
#include <event_groups.h>

#include "bio.h"

#define BIT_1   ( 1 << 1 )
#define BIT_2   ( 1 << 2 )
QueueHandle_t q1; // shared queue between producer and consumer
EventGroupHandle_t et;
TaskHandle_t taskhandle_1;
TaskHandle_t taskhandle_2;

static int cpu_time = 0;

class producer1: public scheduler_task
{
    public:
        producer1(uint8_t priority):scheduler_task("producer", 2048, priority)
        {
            data = 0;
        }

        bool init(void)
        {

            //configUSE_16_BIT_TICKS is set to 0 so
            // that means EventBits_t will have 24 bits, each bit for
            // individual events.
            et = xEventGroupCreate();
            if(et == NULL)
            {
                exit(1); // not able to create eventgroup probably not enough heap memory
            }
            taskhandle_1 = getTaskHandle(); // get and update the taskhandle var.

            return true;
        }
        bool run(void *p)
        {
            for(int i = 0; i < 99; i++)
            {
                // take light sensor data
                data += LS.getRawValue();
                vTaskDelay(1);
            }
            data /= 100;
            xQueueSend(q1, (void*)&data, portMAX_DELAY);


            xEventGroupSetBits(et, BIT_1); // setting bit 1 in eventgroup et

            return true;
        }
    private:
        float data;
};

class consumer1: public scheduler_task
{
    public:
       consumer1(uint8_t priority):scheduler_task("consumer", 2048, priority)
       {
           data = 0;

       }

       bool init(void)
       {
           taskhandle_2 = getTaskHandle();
           return true;
       }

       bool run(void *p)
       {
           float sd_data[10];
           for(int i = 0; i < 10; i++) {
               xQueueReceive(q1, (void*)&data, portMAX_DELAY);
               sd_data[i] = data;
           }
           // file in SD card
           // try to write every 1 sec. into sd card
           //printf("%i, %i\n", time, data); //append the data into sensor.txt
           // L4_IO/fat/disk/diskio.c, look for APIs into that file
           // may be write a function to send 10 unit of data at once to SD card

           xEventGroupSetBits(et, BIT_2); // setting bit 2 in eventgroup et
           return true;
       }
    private:
       float data;
};

class wd_task: public scheduler_task
{
    public:
        wd_task(uint8_t priority):scheduler_task("watchdog_task", 2048, priority)
        {}

        bool init(void)
        {
            return true;

        }

        bool run(void* p)
        {
            //TickType_t bits_for_wait = (BIT_2) | (BIT_1);
            // goto following links to understand eventgroupwaitbits working
            //http://www.freertos.org/xEventGroupWaitBits.html
            EventBits_t final_bit = xEventGroupWaitBits(et, (BIT_2 | BIT_1), pdFALSE, pdTRUE, 1000);
            cpu_time++;
            // open stuctk.txt file in sd card and append the data which task
            // open("stuck.txt");
            // find which task set the bit or timer expired
            if((final_bit & (BIT_2 & BIT_1)) == (BIT_2 & BIT_1))
            {
                // both bits are set by producer and consumer
                printf("both bits are set\n");
            }

            if((final_bit & BIT_2) == BIT_2)
            {
                // only bit 2 was set by consumer
                printf("only bit 2 was set\n");
                // append into file that "task1 stuck"
            }
            if((final_bit & BIT_1) == BIT_1)
            {
                // only producer set the event bit
                printf("only bit 1 was set\n");
                // append into the file that "task2 stuck"
            }
            else
            {
                // timer expired send the data to "stuck.data"
                printf("timer expired\n");
                /*
                 * In the event of failed to detect the bits, append a file
                 * (stuck.txt) with the information about which task may be
                 * "stuck"
                 * */
                // append into the file
                // task1 and task2 both were stuck during 1 second time
                // task1 and task2 stuck
            }

            // close the file here to flush the data into file

            if(cpu_time == 60)
            {
                cpu_time = 0;
                // append cpu_info data into

                //Extra Credit: Every sixty seconds, save the CPU usage info to a file named "cpu.txt".
                //See terminal command "infoHandler" for reference.
            }
            //Open the file, write the file, and close it immediately so the data is immediately flushed.

            return true;
        }
};

int main()
{

    q1 = xQueueCreate(1, sizeof(int));
    // add task command which will work in following way
    //task suspend task1" should suspend task1
    // "task resume task2" should resume task2
    scheduler_add_task(new terminalTask(PRIORITY_MEDIUM));
    scheduler_add_task(new producer1(PRIORITY_MEDIUM));
    scheduler_add_task(new consumer1(PRIORITY_MEDIUM));
    scheduler_add_task(new wd_task(PRIORITY_HIGH));
    scheduler_start();

    return -1;
}