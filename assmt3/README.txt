Please go throught following link to get to know the assignment

http://www.socialledge.com/sjsu/index.php?title=Embedded_System_Tutorial_UART#Assignment

I have added Two source file.

uart1_drv.cpp contains uart1 driver code for the SJ One Board, this should work on any LPC17xx boards
But I have not tested on any other LPC board, let me know if you test this driver on any other LPC17xx 
boards. uart1 supports loopback mode, that means you can test uart1 driver functionality without connecting
any physical wires between two different board. If you guys don't need loopback mode in uart1 driver just 
comment out that instruction in init() function. I have mentioned in the uart1 driver code which line enables 
loopback mode.

uart2_3_drv.cpp contains uart2 and uart3 driver code, but I have commented out init(), run(), get_char2(), 
put_char3() and UART2_IRQHandler(void) function which are part of uart2 driver and only enabled functionality
of uart3 driver. You can try uart2 or uart3 driver code at one time as both drivers have some common function name
only one driver code can be compiled.

I have added interrupt support for uart1, uart2 and uart3 receive data operation and I am not using any rx and tx
fifo, you can add more features if you want to.

For testing uart driver between two SJ one board connect uartn tx to other board rx pin and rx pin of first SJ one 
board to tx of other board, also make sure both the board have connected to common ground. Try to find by yourself 
why we need common ground for both the boards.
