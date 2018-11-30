#include <string.h>
#include <stdio.h>
#include "cpuvariant.h"
#include "emucpu.h"
#include "opcode.h"
#include "disass.h"

void pmx154_reset(struct emuCPU *cpu, bool clearRAM)
{
  if( clearRAM )
    memset( cpu->eMem, 0x55, cpu->maxMem );

  memset( cpu->eIO, 0xFF, cpu->maxIO );

  //setup default IO
  cpu->eIO[0x03] = 0xF4;
  cpu->eIO[0x0D] = 0xFF;
  cpu->eIO[0x0E] = 0xFF;

  ePC = 0; 
  eA = 0; //???
  eGINTEnabled = false;
  eInterruptActive = false;
  eCycle = 0;
  eT16 = 0; //???
}

char* pmx154_ioname(struct emuCPU *cpu, uint8_t io, uint8_t bit, char* buffer)
{
  char* r=0;
  switch( io )
  {
    case 0x00: 
      switch( bit )
      {
        case 0: r="FLAG.ZF"; break;
        case 1: r="FLAG.CF"; break;
        case 2: r="FLAG.AC"; break;
        case 3: r="FLAG.OV"; break;
        default: r="FLAG";
      }
      break;

    case 0x02: r="SP"; break;

    case 0x03: 
      switch( bit )
      {
        case 0: r="CLKMD.PA5 / PRST"; break;
        case 1: r="CLKMD.WD ENABLE"; break;
        case 2: r="CLKMD.ILRC ENABLE"; break;
        case 3: r="CLKMD.CLKTYPE"; break;
        case 4: r="CLKMD.IHRC ENABLE"; break;
        case 5:
        case 6:
        case 7: r="CLKMD.CLKSELECT"; break;
        default: r="CLKMD";
      }
      break;

    case 0x04: 
      switch( bit )
      {
        case 0: r="INTEN PA0"; break;
        case 1: r="INTEN PB0"; break;
        case 2: r="INTEN T16"; break;
        default: r="INTEN";
      }
      break;

    case 0x05: 
      switch( bit )
      {
        case 0: r="INTRQ PA0"; break;
        case 1: r="INTRQ PB0"; break;
        case 2: r="INTRQ T16"; break;
        default: r="INTRQ";
      }
      break;

    case 0x06: r="T16M"; break;

    case 0x08: r="MISC"; break; //differs from CPU to CPU
 
    case 0x0A: r="EOSCR"; break;
    case 0x0B: r="IHRCR"; break;
    case 0x0C: r="INTEGS"; break;
    case 0x0D: if(bit<8) sprintf(buffer,"PADIER.%d",bit); else r="PADIER";  break;
    case 0x0E: if(bit<8) sprintf(buffer,"PBDIER.%d",bit); else r="PBDIER";  break;

    case 0x10: if(bit<8) sprintf(buffer,"PA.%d",bit);  else r="PA";  break;
    case 0x11: if(bit<8) sprintf(buffer,"PAC.%d",bit);  else r="PAC";  break;
    case 0x12: if(bit<8) sprintf(buffer,"PAPH.%d",bit);  else r="PAPH";  break;

    case 0x14: if(bit<8) sprintf(buffer,"PB.%d",bit);  else  r="PB";  break;
    case 0x15: if(bit<8) sprintf(buffer,"PBC.%d",bit);  else  r="PBC";  break;
    case 0x16: if(bit<8) sprintf(buffer,"PBPH.%d",bit);  else  r="PBPH";  break;

    default:
      r="?";
  }

  if( r )
    strcpy(buffer,r);

  return buffer;
}

void pmx154_peripherals(struct emuCPU *cpu)
{
//TODO: INTERRUPT EMU (PC=>[SP];SP+=2;}
//TODO: TIMER EMU
//TODO: WDCOUNTER, ...
//TODO: PWM EMU
//TODO: CLK EMU
//TODO: PA / PB EMU
//TODO: IO 0x3F ==> virtual putchar for easy emu
}



void pmx154_init(struct emuCPU *cpu)
{
  cpu->maxIO = 64;
  cpu->maxMem = 128;
  cpu->maxCode = 2048;
  
  cpu->fnReset = pmx154_reset;
  cpu->fnExecute = opcode14;
  cpu->fnDisassemble = disass14;
  cpu->fnPeripherals = pmx154_peripherals;
  cpu->fnIOName = pmx154_ioname;

  pmx154_reset(cpu, true);
}

