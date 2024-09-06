//usbtouart.cpp
/*
Author(s): Steven Miller
Date created: September 5 2024
Purpose: allows inputs from usb controller(s) and sends out over uart
Notes: when reading this, "usb device" and "usb file" and "usb descriptor" should all
be used interchangably. As usb devices are accessed in linux using file descriptors.
which are just files themselves!
log:
	$ September 5 2024, Steven Miller: Initial creation
	
*/
//TODO
/*
	1.figure out why the gamepads name isnt output unless its unplugged
	2.look into the use of the "select" system call so we dont have to block while waiting for inputs
	3.actually implement reading of joypad inputs when a button is pressed, again, we should try to avoid blocking.
	4.implement uart portion of this whole thing
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/joystick.h>
//directory of file descriptor
#define usb_device_directory "/dev/input/js0"
//check version of joystick driver, it needs to be 1.0 or above to support events
//#ifdef JS_VERSION
//#if JS_VERSION < 0x1
//#endif
//joystick event struct
struct js_event usb_device_event;
 int main()
 {
    char buffer[100]; //buffer for file descriptor data
    int8_t usb_device; //file descriptor

    //open usb file but dont give the ability to write to it
    usb_device = open(usb_device_directory,O_RDONLY);

    //if usb file cannot be opened
    if(usb_device == -1)
    {
        perror("cannot open usb file descriptor");
	close(usb_device);
	return -1;
    }

    std::cout << "gamepad plugged in!\n";
    //read number of axes
    uint8_t usb_device_numberofaxes = 0;
    ioctl(usb_device, JSIOCGAXES, &usb_device_numberofaxes);
    std::cout << usb_device_numberofaxes;
    //get name of joystick. This is really just a sanity test to see if events are working.
    char name[128];
    if(ioctl(usb_device,JSIOCGNAME(sizeof(name)),name) < 0)
	{
	     std::cout << "unknown gamepad \n";
	}
    else
	{
	    name[127] = '\n';
	     std::cout << std::string(name);
	}
    //loop until the usb device is unplugged
  while(read(usb_device,&usb_device_event,sizeof(usb_device_event)) == sizeof(usb_device_event))
{

}
    std::cout << "gamepad unplugged. exiting...\n"; 

    //close the file descriptor
    close(usb_device);
    return 0;
 }


