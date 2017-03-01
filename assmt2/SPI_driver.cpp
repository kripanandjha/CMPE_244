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

#include "LPC17xx.h"
#include "bio.h"
typedef struct
{
        // byte 1
        uint8_t page_size:1;
        uint8_t protect:1;
        uint8_t density:4;
        uint8_t comp:1;
        uint8_t rdy_busy:1;
        //byte 2
        uint8_t erase_suspend_status:1;
        uint8_t program_suspend_status1:1; // buffer 1
        uint8_t program_suspend_status:1; // buffer 2
        uint8_t sector_lockdown:1;
        uint8_t reserved1:1;
        uint8_t erase_prog_error:1;
        uint8_t reserved:1;
        uint8_t rdy_busy1:1;
}ssp1_status_register;

class lab1:public scheduler_task
{
        void cs_enable()
        {
            LPC_GPIO0->FIOCLR |= (0x1 << 6);
        }

        void cs_disable()
        {
            LPC_GPIO0->FIOSET |= (0x1 << 6);
        }
        char byte_transfer(char cmd)
        {
            LPC_SSP1->DR = cmd;
            while(LPC_SSP1->SR & (1 << 4));
            return LPC_SSP1->DR;
        }
    public:
        lab1(uint8_t priority):scheduler_task("spi_drv", 2048,PRIORITY_MEDIUM)
        {}
        bool init(void)
        {

            //ssp1 initialization steps
            //initialize PCONP bit for pcsssp1 to enable ssp1
            LPC_SC->PCONP |= (0x1 << 10); //enable ssp1 co-processor clock
            //for clock set pclksel0 bit pclk_ssp1, for spi master clock
            LPC_SC->PCLKSEL0 &= ~(0x3 << 20);
            LPC_SC->PCLKSEL0 |= (0x1 << 20);// PCLK/1
            // set ssp1 SCK1, MOSI1, MISO1 pin using pinsel mux mode
            LPC_PINCON->PINSEL0 &= ~((0x2 << 14)|(0x2 << 16)|(02 << 18));
            LPC_PINCON->PINSEL0 |= (0x2 << 14)|(0x2 << 16)|(02 << 18);
            //initialize cr0 and cr1 necessary bits
            LPC_SSP1->CR0 = 0x7; // 8 bit data transfer
            LPC_SSP1->CR1 |= (0x1 << 1);

            LPC_SSP1->CPSR &= ~(0xff << 0); // 8 factor by which prescale divide ssp
            LPC_SSP1->CPSR = (1 << 2);
            // peripheral clock

            printf("SSP1 Initialized\n");
            return true;
        }
        bool run(void* param)
        {
            printf("--------------------\n");
            char cmd=0x9f; //cmd to get signature of flash
            //enable_cs
            //LPC_GPIO0->FIOCLR |= (0x1 << 6);
            cs_enable();
            // byte exchange

            byte_transfer(cmd);
            printf("Manufacture ID: %x\n",byte_transfer(cmd));
            printf("Device ID: %x Byte 1\n",byte_transfer(cmd));
            printf("Device ID: %x Byte 2\n",byte_transfer(cmd));
            //LPC_GPIO0->FIOSET |= (0x1 << 6);
            cs_disable();
            //vTaskDelay(500);

            //LPC_GPIO0->FIOCLR |= (0x1 << 6);
            cs_enable();
            cmd = 0xd7;
            byte_transfer(cmd);
            uint8_t sr[2] = {0};
            ssp1_status_register sr_data = {0};
            sr[0] = byte_transfer(cmd);
            sr[1] = byte_transfer(cmd);

            // disable cs
            cs_disable();
            memcpy(&sr_data, &sr, sizeof(sr));
            // byte 1
            printf("Ready/Busy Status %d\n", sr_data.rdy_busy);
            printf("Compare Result %d\n", sr_data.comp);
            printf("Density Code %x\n", sr_data.density);
            printf("sector protection status %d\n", sr_data.protect);
            printf("page size configuration %d\n", sr_data.page_size);

            // byte 2
            printf("Ready/Busy Status %d\n", sr_data.rdy_busy1);
            printf("reserved bit %d\n",sr_data.reserved);
            printf("Erase Program Error %d\n", sr_data.erase_prog_error);
            printf("reserved bit  %d\n",sr_data.reserved1);
            printf("Sector Lockdown error %d\n", sr_data.sector_lockdown);
            printf("Program suspend status %d buffer 2\n", sr_data.program_suspend_status);
            printf("Program suspend status %d buffer 1\n", sr_data.program_suspend_status1);
            printf("erase suspend status %d \n", sr_data.erase_suspend_status);
            printf("flash initial 512 bytes\n\n");

            uint8_t data1[512] = {0};
            uint32_t num_clust;

            flash_read_sectors(data1, 0x00, 1);
            memcpy(&num_clust,&data1[458],4);
            char new_sec = data1[454];
            flash_read_sectors(data1, new_sec, 1);
#if  0
            int i = 0;
            while(i < 512)
            {
                printf("%02x ",data1[i]);
                if(i%16 == 0)
                    printf("\n");
                i++;
            }
#endif
            uint16_t bytes_sector = 0;
            memcpy(&bytes_sector, &data1[11],2);
            printf("no. of bytes per sector %d \n", bytes_sector);
            printf("number of sectors per cluster %d\n",data1[13]);
            printf("total number of sectors %lu\n",num_clust);

            vTaskDelay(2000);

            return true;
        }
};

int main(void)
{

    scheduler_add_task(new lab1(PRIORITY_HIGH));
    scheduler_start();
    return -1;
}
