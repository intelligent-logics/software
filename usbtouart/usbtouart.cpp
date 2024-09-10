//usbtouart.cpp
/* Author(s): Steven Miller, Kieran
Date created: September 5 2024
Date updated: September 10 2024
Purpose: Allows inputs from USB controller(s) and sends out over UART
Notes: USB devices are accessed in Linux using file descriptors, which are just files themselves.
log:
    $ September 5 2024, Steven Miller: Initial creation
    $ September 9 2024, Kieran: Update to enable Axes Reading.
    $ September 10 2024, Kieran: Enabled UART on Raspberry Pi. Able to read controller buttons. 
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
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <string.h>

// Directory of file descriptor
#define USB_DEVICE_DIRECTORY "/dev/input/js0"
#define UART_DEVICE "/dev/serial0" 

// Joystick event struct
struct js_event usb_device_event;

int setup_uart(const char* device) {
    int uart_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd < 0) {
        std::cerr << "Error opening UART device: " << strerror(errno) << std::endl;
        return -1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);

    // Set baud rate
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // 8N1 mode
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // No flow control
    options.c_cflag &= ~CRTSCTS;

    // Turn on READ & ignore ctrl lines
    options.c_cflag |= CREAD | CLOCAL;

    // Turn off software flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Make raw
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    // See: http://unixwiz.net/techtips/termios-vmin-vtime.html
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 0;

    if (tcsetattr(uart_fd, TCSANOW, &options) < 0) {
        std::cerr << "Error setting UART attributes: " << strerror(errno) << std::endl;
        close(uart_fd);
        return -1;
    }

    // Flush port
    tcflush(uart_fd, TCIFLUSH);

    return uart_fd;
}

int main() 
{
    int8_t usb_device; // File descriptor for USB device
    
    // Open USB file but don't give the ability to write to it
    usb_device = open(USB_DEVICE_DIRECTORY, O_RDONLY);
    
    // If USB file cannot be opened
    if (usb_device == -1)
    {
        std::cerr << "Cannot open USB file descriptor: " << strerror(errno) << std::endl;
        return -1;
    }
    
    std::cout << "Gamepad plugged in!\n";

    // Read number of axes
    uint8_t usb_device_numberofaxes = 0;
    ioctl(usb_device, JSIOCGAXES, &usb_device_numberofaxes);
    std::cout << "Number of axes: " << (int)usb_device_numberofaxes << std::endl;

    // Setup UART
    int uart_fd = setup_uart(UART_DEVICE);
    if (uart_fd < 0) {
        close(usb_device);
        return -1;
    }
    std::cout << "UART setup successful\n";

    fd_set readfds;
    struct timeval tv;
    time_t last_check = 0;

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(usb_device, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms timeout

        int ret = select(usb_device + 1, &readfds, NULL, NULL, &tv);

        if (ret > 0) {
            if (read(usb_device, &usb_device_event, sizeof(usb_device_event)) == sizeof(usb_device_event)) {
                // Process the event
                char buffer[32];
                int len;
                switch (usb_device_event.type) {
                    case JS_EVENT_BUTTON:
                        std::cout << "Button " << (int)usb_device_event.number << " " 
                                  << (usb_device_event.value ? "pressed" : "released") << std::endl;
                        // Send over UART
                        len = snprintf(buffer, sizeof(buffer), "B%u:%d\n", usb_device_event.number, usb_device_event.value);
                        if (write(uart_fd, buffer, len) != len) {
                            std::cerr << "UART write error: " << strerror(errno) << std::endl;
                        } else {
                            std::cout << "UART sent: " << buffer;
                        }
                        break;
                    case JS_EVENT_AXIS:
                        std::cout << "Axis " << (int)usb_device_event.number << " at position " 
                                  << usb_device_event.value << std::endl;
                        // Send over UART
                        len = snprintf(buffer, sizeof(buffer), "A%u:%d\n", usb_device_event.number, usb_device_event.value);
                        if (write(uart_fd, buffer, len) != len) {
                            std::cerr << "UART write error: " << strerror(errno) << std::endl;
                        } else {
                            std::cout << "UART sent: " << buffer;
                        }
                        break;
                }
            }
        } else if (ret == 0) {
            // Timeout, could do other processing here
        } else {
            std::cerr << "select() error: " << strerror(errno) << std::endl;
            break;
        }

        // Check gamepad name periodically
        time_t now = time(NULL);
        if (now - last_check > 5) { // Check every 5 seconds
            char name[128];
            if (ioctl(usb_device, JSIOCGNAME(sizeof(name)), name) >= 0) {
                std::cout << "Gamepad: " << name << std::endl;
            } else {
                std::cout << "Gamepad unplugged. Exiting...\n";
                break;
            }
            last_check = now;
        }
    }

    // Close file descriptors
    close(usb_device);
    close(uart_fd);
    return 0;
}