The Assignment is following.

Write two functions with infinite loop that print a large message once a second.
- Use vTaskDelay() to "sleep".
- Do not use printf, instead use uart0_puts() function from #include "uart0_min.h"
- Task1 should print "aaaaaaaaaaaaaaaaaaaaaa" and task 2 should print "bbbbbbbbbbbbbbbb"
Use FreeRTOS xTaskCreate() to create tasks based on your functions.  Use same priority for both tasks.
Note down the observations.
Alter the priority of one of the tasks, and note down the observations.  Tell me WHAT you see and WHY.
How come 4(or 3 sometimes) characters are printed from each task?  Why not 2 or 5, or 6?


I won't be explaining the answer as I don't want to spoil the fun. I will just give the small hint

In our course Freertos project code given was given with preconfigured with 1ms systick timer valus
and uart0 which is printing data on serial console will print print in 1ms whatever character it can 
print based on configures baud-rate set.
