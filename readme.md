*****************************************************************************
ChibiOS/RT base devlopment code for F405 Board      
*****************************************************************************

 This repo contains all files for a clean development on a STM32 F405 board. 
 It includes files in C and C++ to show example of multiple language compilation, as well as a example files for :
    -- usb_otg/* to create a virtual com port on the FS USB port of the board
    
    -- shel_conf.c/h to create a shell engine in the chip whcih answers basic commands (threads, mem, turn on and off LED)
    
    -- pwm_conf.h to enable PWM
    
    -- spi_conf.h to enable SPI with 3 differents clock speed
    
    -- an example of a C++ library wrapper
    
  It runs on ChibiOS 19.1 but should be easily ported to 18.2 as the project was originally created on that branch.
  
  Just modify the makefile variable CHIBIOS to adjust the location of your chibiOS source files.
  
To use SPI com with icmu :

     -- bspi sends a basic frame to check connectivity of the icmu without using ichausmu library (just reading the HW revision of the board)
     -- ispi launches the 5 request to initialize the icmu and provide a command feedback for each commmand (feedback = 0 is good)
     -- spi launchs a position command reading
     
     
  
  


