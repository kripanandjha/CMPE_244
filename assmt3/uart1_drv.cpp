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
extern "C"{
void receive_char(char data)
{
    u0_dbg_printf("%c",data); // printing the data got from RBR of uart1 
}
void UART1_IRQHandler(void)
{

    receive_char(LPC_UART1->RBR);
}
}
class uart2_3: public scheduler_task
{
    public:
        uart2_3(uint8_t priority):scheduler_task("uart2_3", 2048, priority)
        {}

        bool init(void)
        {

            uint32_t uart1_clock = (0x1 << 4);
            uint32_t uart1_clk_mul = (0x1 << 8);
            uint32_t uart1_tx = 0x2; //port2.0:1
            uint32_t uart1_rx = 0x2 << 2; // port2.1:2
            // init your uart here

            // 1. enable uart1 co-processor clock
            LPC_SC->PCONP |= uart1_clock;

            // 2. Enable uart peripheral clock div we are using div = 1
            LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(0x3 << 8))
                              | uart1_clk_mul;

            uint32_t enable_dlab = (0x1 << 7);
            //uint32_t disable_dlab = ~(0x1 << 7);

            uint32_t bit_8 = 0x3;
            // 3. set the baud rate
            // we want to set baud rate to 115200
            // I. first enable DLAB bit in uart2_lcr to access DLL and DLM divisor
            LPC_UART1->LCR |= enable_dlab; // by default 0 is 1 stop bit so no need to set it
            {
                uint32_t baud_rate = 115200;

                uint16_t data = sys_get_cpu_clock()/(16*baud_rate)+0.5;
                printf("system clock %d", data);

                LPC_UART1->DLL = data;
            }
            LPC_UART1->LCR = bit_8;

            LPC_UART1->FCR = 1; // enable rx fifo

            LPC_UART1->MCR |= (1 << 4); // enable loopback mode

            //4. select uart2 rx, tx pin, need to add common of both the boards
            LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(0x3))
                               | uart1_tx;

            LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(0x3 << 2))
                               | uart1_rx;
            LPC_UART1->IER = 0x1; // enable interrupt for RBR
            NVIC_EnableIRQ(UART1_IRQn); // setting interrupt level in NV interrupt controller

            return true;
        }

        char get_char(void)
        {
            while(!(LPC_UART1->LSR & 0x1));
                return LPC_UART1->RBR;
        }

        void put_char(char ch)
        {
            while(!(LPC_UART1->LSR & (0x1 << 5)));
                LPC_UART1->THR = ch; // will put into lsb 8 bit into THR

        }
        bool run(void* p)
        {
            char c;
            scanf("%c",&c); // gets the data from the user
            put_char(c);// sends the data to uart1 transmit holding register
            return true;
        }
};
int main(void)
{

    scheduler_add_task(new uart2_3(PRIORITY_HIGH));
    scheduler_start();
    return -1;
}
