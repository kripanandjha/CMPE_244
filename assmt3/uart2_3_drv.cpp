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
#include "printf_lib.h"
#include "examples/examples.hpp"

#include "LPC17xx.h"
#include "bio.h"
extern "C"{
void receive_char(char data)
{
    u0_dbg_printf("%c",data);
}

#if 0
void UART2_IRQHandler(void)
{

    receive_char(LPC_UART2->RBR);
}
#endif
#if 1
void UART3_IRQHandler(void)
{

    receive_char(LPC_UART3->RBR);
}
#endif
}
class uart2_3: public scheduler_task
{
    public:
        uart2_3(uint8_t priority):scheduler_task("uart2_3", 2048, priority)
        {}
#if 0
		// uart2 co-processor initialization
        bool init(void)
        {
            uint32_t uart2_clock = (0x1 << 24);
            uint32_t uart2_clk_mul = (0x1 << 16);
            uint32_t reset_uart2_clk = ~(0x3 << 16);
            uint32_t uart2_tx = (0x2 << 16); // port2.8
            uint32_t uart2_rx = (0x2 << 18); // port2.9
            // init your uart here

            // 1. initialize uart2 co-processor clock
            LPC_SC->PCONP |= uart2_clock;

            // 2. Enable uart peripheral clock div we are using div = 1
            LPC_SC->PCLKSEL1 = (LPC_SC->PCLKSEL1 & reset_uart2_clk)
                             | uart2_clk_mul;

            uint32_t enable_dlab = (0x1 << 7);
            //uint32_t disable_dlab = ~(0x1 << 7);

            uint32_t bit_8 = 0x3;
            // 3. set the baud rate
            // we want to set baud rate to 115200
            // I. first enable DLAB bit in uart2_lcr to access DLL and DLM divisor
            LPC_UART2->LCR = enable_dlab; // by default 0 is 1 stop bit so no need to set it
            {
                uint32_t baud_rate = 115200;

                uint16_t data = sys_get_cpu_clock()/(16*baud_rate)+0.5;

                LPC_UART2->DLL = data; // lsb 8 bit data
                LPC_UART2->DLM = data >> 8; // msb 8 bit
            }
            LPC_UART2->LCR = bit_8; //disable dlab and set 8 bit mode
            //LPC_UART2->FCR = 1; //enable fifo

            //4. select uart2 rx, tx pin, need to add common of both the boards
            LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(0x3 << 16))
                                 | uart2_tx;

            LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(0x3 << 18))
                                 | uart2_rx;
            LPC_UART2->IER = 0x1; // enable interrupt for RBR
            NVIC_EnableIRQ(UART2_IRQn);
            return true;
        }
#endif
#if 1
		// uart3 co-processor initialization 
        bool init(void)
        {
            uint32_t uart3_clock = (0x1 << 25); ///uart 3 clock
            uint32_t uart3_clk_mul = (0x1 << 18); // setting /1 mul
            uint32_t uart3_tx = (0x3 << 24); // port 4.28
            uint32_t uart3_rx = (0x3 << 26); // port 4.29

            // uint32_t uart3_tx = (0x2); // port 0.0:1
            // uint32_t uart3_rx = (0x2 << 2); // port 0.2:3

            // init your uart here

            // 1. enable uart3 co-processor clock
            LPC_SC->PCONP |= uart3_clock;

            // 2. Enable uart peripheral clock div we are using div = 1
            LPC_SC->PCLKSEL1 = (LPC_SC->PCLKSEL1 & ~(0x3 << 18))
                                             | uart3_clk_mul;

            uint32_t enable_dlab = (0x1 << 7);

            uint32_t bit_8 = 0x3;
            // 3. set the baud rate
            // we want to set baud rate to 115200
            // I. first enable DLAB bit in uart3_lcr to access DLL and DLM divisor
            LPC_UART3->LCR |= enable_dlab; // by default 0 is 1 stop bit so no need to set it
            {
                uint32_t baud_rate = 115200;

                uint16_t data = (sys_get_cpu_clock()/(16*baud_rate))+0.5;

                LPC_UART3->DLL = data; // data lsb 8 bit
                LPC_UART3->DLM = data >> 8; // data msb 8 bit
            }

            LPC_UART3->LCR = bit_8; // disable dlab and set 8 bit
            //LPC_UART3->FCR = 1; // enable fifo

            // 4. select uart2 rx, tx pin, need to add common of both the boards
            LPC_PINCON->PINSEL9 |= uart3_tx;

            LPC_PINCON->PINSEL9 |= uart3_rx;
            LPC_UART3->IER = 0x1; // enable interrupt for RBR
            NVIC_EnableIRQ(UART3_IRQn);
            return true;
        }
#endif
#if 0
		// if not using interrupt use the following uart2 polling function
        char get_char2(void)
        {
            while(!(LPC_UART2->LSR & 0x1));

            return (LPC_UART2->RBR & 0xff);
        }

        void put_char2(char ch)
        {
            while(!(LPC_UART2->LSR & (0x1 << 5)));

            LPC_UART2->THR = ch; // will put into lsb 8 bit into THR

        }
#endif
#if 1
		// if not using interrupt use the following uart2 polling function
        char get_char3(void)
        {
            while(!(LPC_UART3->LSR & 0x1));

            return (LPC_UART3->RBR & 0xff);
        }

        void put_char3(char ch)
        {
            while(!(LPC_UART3->LSR & (0x1 << 5)));

            LPC_UART3->THR = ch; // will put into lsb 8 bit into THR

        }
#endif
        bool run(void* p)
        {
            char c='M';
            //scanf("%c",&c);
            put_char3(c);
            //put_char2(c);
            vTaskDelay(1000);
            //u0_dbg_printf("%c\n",get_char3());
            return true;
        }
};
int main(void)
{

    scheduler_add_task(new uart2_3(PRIORITY_HIGH));
    scheduler_start();
    return -1;
}
