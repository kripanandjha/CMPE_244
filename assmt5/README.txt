The aim of this assignment is the following 
Write two functions with infinite loop that print a large message once a second.
- Use vTaskDelay() to "sleep".
- Do not use printf, instead use uart0_puts() function from #include "uart0_min.h"
- Task1 should print "aaaaaaaaaaaaaaaaaaaaaa" and task 2 should print "bbbbbbbbbbbbbbbb"
Use FreeRTOS xTaskCreate() to create tasks based on your functions.  Use same priority for both tasks.
Note down the observations.
Alter the priority of one of the tasks, and note down the observations.  Tell me WHAT you see and WHY.
How come 4(or 3 sometimes) characters are printed from each task?  Why not 2 or 5, or 6?
