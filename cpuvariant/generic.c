#include <string.h>
#include <stdio.h>
#include "cpuvariant.h"
#include "emucpu.h"
#include "opcode.h"
#include "disass.h"

void generic_reset(struct emuCPU *cpu, bool clearRAM)
{
}

char* generic_ioname(struct emuCPU *cpu, uint8_t io, uint8_t bit, char* buffer)
{
  strcpy(buffer,"?");
  return buffer;
}

void generic_peripherals(struct emuCPU *cpu)
{
}

int generic_init(struct emuCPU *cpu)
{
  cpu->fnReset = generic_reset;
  cpu->fnPeripherals = generic_peripherals;
  cpu->fnIOName = generic_ioname;

  switch( cpu->hdr.codesize )
  {
    case 1024:
      cpu->maxIO = 32;
      cpu->maxMem = 60;
      cpu->maxCode = 1024;
      cpu->fnExecute = opcode13;
      cpu->fnDisassemble = disass13;
      break;
  
    case 2048:
      cpu->maxIO = 32;
      cpu->maxMem = 128;
      cpu->maxCode = 2048;
      cpu->fnExecute = opcode14;
      cpu->fnDisassemble = disass14;
      break;
  
    case 4096:
      cpu->maxIO = 64;
      cpu->maxMem = 256;
      cpu->maxCode = 4096;
      cpu->fnExecute = opcode15;
      cpu->fnDisassemble = disass15;
      break;
  
    case 8192:
      cpu->maxIO = 32;
      cpu->maxMem = 512;
      cpu->maxCode = 8192;
      cpu->fnExecute = opcode16;
      cpu->fnDisassemble = disass16;
      break;

    default:
      return -1;
  }
  return 0;
}

