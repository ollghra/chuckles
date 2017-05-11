#include <stdint.h>
#include <stdio.h>

#include <sys/io.h>
#include <arch/i386/ps2.h>

uint8_t status;

char *devices[5] = {
  "PS/2 mouse without scroll wheel",
  "Mouse with scroll wheel",
  "5-button mouse",
  "MF2 keyboard with traslation enabled :(",
  "MF2 keyboard without translation B-)"
};

#define STATUS_OUT_FULL 1
#define STATUS_IN_FULL (1 << 1)

/**
   Waits for status bit to set
   Reads from data port 
*/
uint8_t ps2_read(void)
{
  do
    status = inb(STATUS_PORT); 
  while(!(status & STATUS_OUT_FULL));
  uint8_t data = inb(DATA_PORT);
  status = inb(STATUS_PORT);
  return data;
}

/**
   port: 1 if port 1, 2 if port 2
*/
void ps2_write(uint8_t port, uint8_t data)
{
  if(port==2)
    outb(CMD_PORT, WRITE_P2);
  do
    status = inb(STATUS_PORT);
  while(status & STATUS_IN_FULL);
  outb(DATA_PORT, data);
}

void ps2_init(void)
{
  struct{
    uint8_t num_ports;
  } ps2_info;
  ps2_info.num_ports = 2;
  
  ps2_write(CMD_PORT, 0xAD); // Disable 1
  ps2_write(CMD_PORT, 0xA7); // Disable 2

  for(status = inb(STATUS_PORT);
      status & STATUS_OUT_FULL;
      status = inb(STATUS_PORT))
    inb(DATA_PORT);          // Flush output buffer

  // Set Controller Configuration Byte
  outb(CMD_PORT, 0x20);
  uint8_t config = ps2_read();
  if(config & (1<<5)) {// If b5 set, PS/2 not 2 channel
    printf("%d: config byte = %x\n", __LINE__,config);
    printf("PS/2 single channel\n");
    ps2_info.num_ports = 1;
  }
  
  config &= 0xBC; // Bits 0,1,6: irqs for 1 and 2, disable translationa
  printf("%d: config byte = %x\n", __LINE__,config);
  outb(CMD_PORT, 0x60);
  ps2_write(1, config);
  
  // Controller Self-Test
  outb(CMD_PORT, 0xAA);
  if(ps2_read() != 0x55)
    printf("PS/2 controller self-test failed\n");
  
  // 2 Channel test
  if(ps2_info.num_ports == 2)
    {
      ps2_write(2, 0xA8);
      printf("line: %d\n", __LINE__);
      config = ps2_read(); // BLOCKING HERE
      printf("line: %d\n", __LINE__);
      if(config & (1<<5))
	{
	  printf("PS/2 single channel confirmed\n");
	  ps2_info.num_ports = 1;
	}
    };
  outb(CMD_PORT, 0xA7);
  
  // Device Interface tests
  outb(CMD_PORT, 0xAB);
  uint8_t ret_code = ps2_read();
  if(ret_code)
    printf("PS/2 port 1 test failed with code %x\n", ret_code);

  if(ps2_info.num_ports == 2)
    {
      outb(CMD_PORT, 0xA9);
      ret_code = ps2_read();
      if(ret_code)
	printf("PS/2 port 2 test failed with code %x\n", ret_code);
    }

  // Re-enabling IRQs for 1 and 2
  outb(CMD_PORT, 0x20);
  config = ps2_read();
  config |= 0x3; // Bits 0,1: irqs on for 1 and 2
  outb(CMD_PORT, 0x60);
  ps2_write(1, config);
  
  // Enabling Devices
  ps2_write(1, 0xAE);
  if(ps2_info.num_ports == 2)
    ps2_write(2, 0xA8);

  // Reset Devices
  ps2_write(1, 0xFF);
  printf("Response to RESET from port 1: %x\n", ps2_read());

  if(ps2_info.num_ports == 2)
    {
      ps2_write(2, 0xFF);
      printf("Response to RESET from port 2: %x\n", ps2_read());
    }

  printf("Detecting device types\n");
  ps2_write(1, 0xF5);
  while (ps2_read() != 0xFA);
  ps2_write(1, 0xF2);

  for(status = ps2_read(); status != 0xFA; status = ps2_read());
  ps2_read();
  status = ps2_read();
  switch(status)
    {
    case 0:
      printf("Device is %s\n", devices[0]);
      break;
    case 0x3:
      printf("Device is %s\n", devices[1]);
      break;
    case 0x4:
      printf("Device is %s\n", devices[2]);
      break;
    case 0xAB:
      switch(ps2_read())
	{
	case 0x41:
	case 0xC1:
	  printf("Device is %s\n", devices[3]);
	  break;
	case 0x83:
	  printf("Device is %s\n", devices[4]);
	  break;
	}
      break;
    default:
      printf("Unknown device type 8-o\n");
      break;
    }
  ps2_write(1, 0xF4);
  printf("Scanning enabled %s\n", ps2_read() == 0xFA ? "Yes": "Error");
}

