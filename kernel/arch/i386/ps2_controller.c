#include <stdint.h>
#include <stdio.h>

#include <sys/io.h>
#include <arch/i386/ps2.h>

uint8_t status;

char *devices[5] = {
  "PS/2 mouse without scroll wheel",
  "Mouse with scroll wheel",
  "5-button mouse",
  "MF2 keyboard with translation enabled :(",
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
  printf("PS/2 CONTROLLER STILL DOESN'T WORK\n");
  struct{
    uint8_t num_ports;
  } ps2_info;
  ps2_info.num_ports = 2;
  
  ps2_write(CMD_PORT, 0xAD); // Disable 1
  ps2_write(CMD_PORT, 0xA7); // Disable 2

  for(status = inb(STATUS_PORT);
      status & STATUS_OUT_FULL;
      status = inb(STATUS_PORT))
    inb(DATA_PORT);          // Flush output buffer?

  // Set Controller Configuration Byte
  outb(CMD_PORT, 0x20);
  uint8_t config = ps2_read();
  if(config & (1<<5)) {// If b5 set, PS/2 not 2 channel
    printf("%d: config byte = %x\n", __LINE__,config);
    printf("PS/2 single channel\n");
    ps2_info.num_ports = 1;
  }
  else
    {
      ps2_info.num_ports = 2;
    }
  printf("/%d: before, config byte = %x\n", __LINE__,config);
  config &= 0x3F; // Bits 0,1,6: irqs for 1 and 2, disable translationa
  printf("\\%d: after , config byte = %x\n", __LINE__,config);
  do
    {
      status = inb(STATUS_PORT);
      printf("poll ");
    }
  while(status & STATUS_IN_FULL);
  outb(CMD_PORT, 0x60);
  ps2_write(1, config);
  
  int retries = 0;
/*
#undef qemu
#ifndef qemu
  for(status = ps2_read(); status != 0xFA && retries < 1; status = ps2_read(),retries++)
    {
      printf("\nstatus = %x ", status);
      if(status == 0xFE)
	{
	  do
	    {
	      status = inb(STATUS_PORT);
	      printf("poll ");
	    }
	  while(status & STATUS_IN_FULL);
	  outb(CMD_PORT, 0x60);
	  ps2_write(1, config);
	}
    }
#endif
  */
  outb(CMD_PORT, 0x20);
  printf("/%d: before, config byte = %x\n", __LINE__,config);
  config = ps2_read();
  printf("\\%d: after,  config byte = %x\n", __LINE__,config);
  
  // Controller Self-Test
  outb(CMD_PORT, 0xAA);
  status = ps2_read();
  for(;status == 0xFE;)
    {
      status = ps2_read();
    }
  if(status != 0x55)
    {
      printf("PS/2 controller self-test failed with code %x\n", status);
    }
  // 2 Channel test
  if(ps2_info.num_ports == 2)
    {
      ps2_write(2, 0xA8);
      printf("line: %d\n", __LINE__);
      outb(CMD_PORT, 0x20);
      config = ps2_read(); // BLOCKING HERE
      printf("line: %d\n", __LINE__);
      if(config & (1<<5))
	{
	  printf("PS/2 single channel confirmed\n");
	  ps2_info.num_ports = 1;
	  outb(CMD_PORT, 0xA7);
	}
    };

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
  status = ps2_read();
  retries = 0;
  for(;status != 0xFA && retries < 2;retries++)
    {
      printf("Responded %x. Retrying...", status);
      status = ps2_read();
    }
  printf("Response to RESET from port 1: %x\n", status);

  if(ps2_info.num_ports == 2)
    {
      ps2_write(2, 0xFF);
      printf("Response to RESET from port 2: %x\n", ps2_read());
    }

  printf("Detecting device types\n");
  ps2_write(1, 0xF5);
  for(status = ps2_read(); status != 0xFA; status = ps2_read())
    printf("status: %x ", status);
  ps2_write(1, 0xF2);
  printf("Request acknowleged\n");

  for(status = ps2_read(); status != 0xFA; status = ps2_read())
    printf("status:%x ", status);
  status = ps2_read();
  printf("Response: %x\n", status);
  if(status != 0x0 && status != 0xAB)
    {
      status = ps2_read();
      printf("Status: %x\n", status);
    }

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
      printf("Unknown device type %x\n", status);
      break;
    }
  ps2_write(1, 0xF4);

  printf("Scanning enabled: %x\n", ps2_read());

  printf("/%d: before, config byte = %x\n", __LINE__,config);
  config &= 0x35; 
  printf("\\%d: after , config byte = %x\n", __LINE__,config);
  do
    {
      status = inb(STATUS_PORT);
      printf("poll ");
    }
  while(status & STATUS_IN_FULL);
  outb(CMD_PORT, 0x60);
  ps2_write(1, config);

  outb(CMD_PORT, 0x20);
  printf("/%d: before, config byte = %x\n", __LINE__,config);
  config = ps2_read();
  printf("\\%d: after,  config byte = %x\n", __LINE__,config);
  
}

/*
outb(CMD_PORT, 0x60);
  config &= 0x3F;
  ps2_write(1, config);

  outb(CMD_PORT, 0x20);
  printf("/%d: before, config byte = %x\n", __LINE__,config);
  config = ps2_read();
  printf("\\%d: after,  config byte = %x\n", __LINE__,config);
  config = ps2_read();
  printf("\\%d: after,  config byte = %x\n", __LINE__,config);
  
 */
