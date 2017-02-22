All these assignment files are part of free software framework for embedded processors.
To use these assigment files in your projects, you can download the sample project
source along with toolchain from the following link

https://sourceforge.net/projects/armdevpkg/files/

If you need any help in setting development environment for the project
please go through following link

http://www.socialledge.com/sjsu/index.php?title=Development_Package


All these assignment are made for the LPC1758 board and
should work in freertos environment.

After successfully setting development environment and
able to flash your sample projects in the board. Now you can replace
the main.cpp file in the L5_Application folder of your sample project
with assmt1.cpp and don't forget to rename the file assmt1.c file with
main.cpp.

Assignment - 1
The Assignemtn 1 is to interface the external led and switch to the 
gpio pin of the board and write a program to glow the led based on 
the switch operation. after doing this task you can add some feature 
like glowing the led by sending a command on serial termainal from your
system to the board.

PIN conf.
The assmt1.c assumes led is connected on the port 2 pin 7 
and switch is connected to the port 2 pin 6

Note: you can either pull up or pull down resistor either way switch
will work but in opposite way.
