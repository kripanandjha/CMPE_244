Assignment 7:-

-- Create a task that polls the acceleration sensor, and determines the orientation of the board, such as "up, down, left, right".  Create an enumeration such as "typdef enum { invalid, up, down, left, right, }orientation_t;
-- Create a task that is waiting on the orientation enumeration to be sent by the previous task.
-- Create a queue, and have the first task send orientation values every second to the queue.
-- Print something BEFORE and AFTER sending the enumeration value to the queue.
-- Print something immediately after the second task receives the data from the queue.
-- Use same priority for both tasks and note down the print-outs.
-- Alter the priority of second task to use higher priority, and note down the print-outs

Further details and examples can be located here:

http://www.socialledge.com/sjsu/index.php?title=Embedded_System_Tutorial_FreeRTOS (Links to an external site.)
