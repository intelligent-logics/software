/* Author(s):  Kieran Abesamis
Date created: October 2nd, 2024
Purpose: Tests usbtouart.cpp
Notes: Assited by Claude.ai to make tests.
    $ S
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <cmath>

// Constants from the original code
#define USB_DEVICE_DIRECTORY "/dev/input/js0"
#define UART_DEVICE "/dev/ttyAMA10"
#define BAUDRATE B115200
#define A_BITS      0b00000001
#define B_BITS      0b00000010
#define SELECT_BITS 0b00000100
#define START_BITS  0b00001000
#define UP_BITS     0b00010000
#define DOWN_BITS   0b00100000
#define LEFT_BITS   0b01000000
#define RIGHT_BITS  0b10000000

// Function prototypes
int setup_uart(const char* device);
void process_event(const js_event& event, uint8_t* controller_byte);
uint8_t construct_uart_string(const uint8_t* controller_byte);

// Mock functions
int mock_open(const char* pathname, int flags) {
    return 1; // Return a dummy file descriptor
}

int mock_close(int fd) {
    return 0; // Always succeed
}

int mock_tcgetattr(int fd, struct termios *termios_p) {
    return 0; // Always succeed
}

int mock_tcsetattr(int fd, int optional_actions, const struct termios *termios_p) {
    return 0; // Always succeed
}

int mock_tcflush(int fd, int queue_selector) {
    return 0; // Always succeed
}

// Test setup_uart function
void test_setup_uart() {
    // Replace actual system calls with mocks
    #define open mock_open
    #define close mock_close
    #define tcgetattr mock_tcgetattr
    #define tcsetattr mock_tcsetattr
    #define tcflush mock_tcflush

    int result = setup_uart(UART_DEVICE);
    assert(result >= 0 && "setup_uart should return a non-negative file descriptor");

    #undef open
    #undef close
    #undef tcgetattr
    #undef tcsetattr
    #undef tcflush

    std::cout << "test_setup_uart passed\n";
}

// Test process_event function
void test_process_event() {
    uint8_t controller_byte[8] = {0};
    js_event event;

    // Test A button press
    event.type = JS_EVENT_BUTTON;
    event.number = 1;
    event.value = 1;
    process_event(event, controller_byte);
    assert(controller_byte[0] == 1 && "A button press should set controller_byte[0] to 1");

    // Test B button release
    memset(controller_byte, 0, sizeof(controller_byte));
    event.type = JS_EVENT_BUTTON;
    event.number = 2;
    event.value = 0;
    process_event(event, controller_byte);
    assert(controller_byte[1] == 0 && "B button release should set controller_byte[1] to 0");

    // Test Up direction
    memset(controller_byte, 0, sizeof(controller_byte));
    event.type = JS_EVENT_AXIS;
    event.number = 1;
    event.value = -32767;
    process_event(event, controller_byte);
    assert(controller_byte[4] == 1 && "Up direction should set controller_byte[4] to 1");

    // Test Right direction
    memset(controller_byte, 0, sizeof(controller_byte));
    event.type = JS_EVENT_AXIS;
    event.number = 0;
    event.value = 32767;
    process_event(event, controller_byte);
    assert(controller_byte[7] == 1 && "Right direction should set controller_byte[7] to 1");

    std::cout << "test_process_event passed\n";
}

// Test construct_uart_string function
void test_construct_uart_string() {
    uint8_t controller_byte[8] = {1, 1, 0, 0, 1, 0, 0, 1}; // A, B, Up, Right pressed
    uint8_t expected_uart_string = A_BITS | B_BITS | UP_BITS | RIGHT_BITS;

    uint8_t actual_uart_string = construct_uart_string(controller_byte);
    assert(actual_uart_string == expected_uart_string && "UART string construction failed");

    std::cout << "test_construct_uart_string passed\n";
}

// Helper function to implement process_event
void process_event(const js_event& event, uint8_t* controller_byte) {
    if(event.type == JS_EVENT_BUTTON) {
        switch(event.number) {
            case 1: // A button
                controller_byte[0] = event.value;
                break;
            case 2: // B button
                controller_byte[1] = event.value;
                break;
            case 8: // Select button
                controller_byte[2] = event.value;
                break;
            case 9: // Start button
                controller_byte[3] = event.value;
                break;
        }
    } else if(event.type == JS_EVENT_AXIS) {
        if(event.number == 1) { // Up/Down axis
            if(event.value == -32767) { // Up
                controller_byte[4] = 1;
                controller_byte[5] = 0;
            } else if(event.value == 32767) { // Down
                controller_byte[4] = 0;
                controller_byte[5] = 1;
            } else { // Neither
                controller_byte[4] = 0;
                controller_byte[5] = 0;
            }
        } else if(event.number == 0) { // Left/Right axis
            if(event.value == -32767) { // Left
                controller_byte[6] = 1;
                controller_byte[7] = 0;
            } else if(event.value == 32767) { // Right
                controller_byte[6] = 0;
                controller_byte[7] = 1;
            } else { // Neither
                controller_byte[6] = 0;
                controller_byte[7] = 0;
            }
        }
    }
}

// Helper function to implement construct_uart_string
uint8_t construct_uart_string(const uint8_t* controller_byte) {
    uint8_t uart_string = 0;
    for(uint8_t i = 0; i < 8; i++) {
        if(controller_byte[i] == 1) {
            uart_string += static_cast<uint8_t>(pow(2, i));
        }
    }
    return uart_string;
}

int main() {
    // Run all tests
    test_setup_uart();
    test_process_event();
    test_construct_uart_string();

    std::cout << "All tests passed!\n";
    return 0;
}
