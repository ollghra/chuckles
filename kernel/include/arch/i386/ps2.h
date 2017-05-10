#ifndef ARCH_I386_PS2_H
#define ARCH_I386_PS2_H

#define DATA_PORT 0x60
#define STATUS_PORT 0x64
#define CMD_PORT 0x64

#define WRITE_P2 0xD4

uint8_t ps2_read(void);

/**
   port: 1 if port 1, 2 if port 2
*/
void ps2_write(uint8_t port, uint8_t data);

void ps2_init(void);

#endif
