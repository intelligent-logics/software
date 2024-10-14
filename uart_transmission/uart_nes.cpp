#include <iostream>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fstream>
#include <vector>
#include <fcntl.h>
#include <stdint.h>

#define PRG_16KB 16384
#define CHR_8KB 8192
#define HANDSHAKE 0xFE
#define PROG_START 0x8000
#define CHAR_START 0x0000

int main(){
    
    // Start with reading the bytes of the .nes file
    std::ifstream infile("game.nes", std::ios::binary | std::ios::ate);

    if(!file){
         std::cout << "Failed to open file\n";
         return -1
    }

    // Get file size and return to start
    std::streamsize fileSize = infile.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);

    if(file.read(buffer.data), fileSize)    std::cout << "Succesfully read all bytes\n";
    else                                    std::cout << "Failed to read bytes\n";
        
    int prog_rom_size = PRG_16KB * (uint8_t)buffer[4];
    int char_rom_size = CHR_8KB * (uint8_t)buffer[5];


    // Initialize UART
    int fd = -1;
    fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);

    if(fd == -1)    return -1;
    
    struct termios options;
    tcgetattr(fd, &options);

    // Configure UART
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    // Step 3: Transmit handshake
    uint8_t hs = HANDSHAKE;
    int send = write(fd, &hs, 1);

    if(!send)   return -1;


    // Step 4: Await ACK

    int ack = -16;

    while(ack != HANDSHAKE)
        ack = read(fd, &ack, 1);


    // Step 5: Send data
    uint8_t packet = 0;
    int status = -255;
    int addr = 0;
    uint8_t addr_lo = 0;
    uint8_t addr_hi = 0;

    // Start with PRG ROM

    for(int i = 0; i < prog_rom_size; i++){

        addr = PROG_START + i;

        // Send data
        packet = (uint8_t)buffer[16 + i];
        status = write(fd, &packet, 1);

        // Send low byte
        addr_lo = (uint8_t)(addr & 0xFF);
        status = write(fd, &addr_lo, 1);

        // Send high byte

        addr_hi = (uint8_t)((addr >> 8) & 0xFF);
        status = write(fd, &addr_hi, 1);

    }

    // Now do the CHR ROM
    // Offest from the header and program memory size
    int char_offset = 16 + prog_rom_size;

    for(int i = 0; i < char_rom_size; i++){

        addr = CHAR_START + i;

        // Send data
        packet = (uint8_t)buffer[char_offset + i];
        status = write(fd, &packet, 1);

        // Send low byte
        addr_lo = (uint8_t)(addr & 0xFF);
        status = write(fd, &addr_lo, 1);

        // Send high byte

        addr_hi = (uint8_t)((addr >> 8) & 0xFF);
        status = write(fd, &addr_hi, 1);

    }


    // Terminate
    int send = write(fd, &hs, 1);

    return 0;
}