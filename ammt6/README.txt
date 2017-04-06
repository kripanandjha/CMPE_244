The Assignment is following.

-- Attach a switch to PORT2
-- Hook up an interrupt function when the switch is pressed.  Use the API at eint.h
-- Create a binary semaphore.
-- When the function is called back using interrupt, "give" a semaphore.
-- Create a task that is waiting on the semaphore, and prints a message when the semaphore is given.
-- Do your best to tackle the "switch debounce".  Hint: Use a timer, and once a semaphore is given, do not give another semaphore for another 50ms or so.  
-- The Wikipedia board page has information about the timer services.
