#include <string.h>
#include <stdio.h>
#include "cpuvariant.h"
#include "emucpu.h"
#include "opcode.h"
#include "disass.h"

void pmx150_reset(struct emuCPU *cpu, bool clearRAM)
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

char* pmx150_ioname(struct emuCPU *cpu, uint8_t io, uint8_t bit, char* buffer)
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
        case 2: r="INTEN T16"; break;
        default: r="INTEN";
      }
      break;

    case 0x05: 
      switch( bit )
      {
        case 0: r="INTRQ PA0"; break;
        case 2: r="INTRQ T16"; break;
        default: r="INTRQ";
      }
      break;

    case 0x06: r="T16M"; break;

    case 0x1B: r="MISC"; break; //differs from CPU to CPU
 
    case 0x0B: r="IHRCR"; break;
    case 0x0C: r="INTEGS"; break;
    case 0x0D: if(bit<8) sprintf(buffer,"PADIER.%d",bit); else r="PADIER";  break;

    case 0x10: if(bit<8) sprintf(buffer,"PA.%d",bit);  else r="PA";  break;
    case 0x11: if(bit<8) sprintf(buffer,"PAC.%d",bit);  else r="PAC";  break;
    case 0x12: if(bit<8) sprintf(buffer,"PAPH.%d",bit);  else r="PAPH";  break;

    default:
      r="?";
  }

  if( r )
    strcpy(buffer,r);

  return buffer;
}

void pmx150_peripherals(struct emuCPU *cpu)
{
//TODO: INTERRUPT EMU (PC=>[SP];SP+=2;}
//TODO: TIMER EMU
//TODO: WDCOUNTER, ...
//TODO: PWM EMU
//TODO: CLK EMU
//TODO: PA / PB EMU
//TODO: IO 0x3F ==> virtual putchar for easy emu
}

void pmx150_init(struct emuCPU *cpu, bool fixupHighCode)
{
  cpu->maxIO = 32;
  cpu->maxMem = 60;
  cpu->maxCode = 1024;
  
  cpu->fnReset = pmx150_reset;
  cpu->fnExecute = opcode13;
  cpu->fnDisassemble = disass13;
  cpu->fnPeripherals = pmx150_peripherals;
  cpu->fnIOName = pmx150_ioname;

  if( fixupHighCode )
  {
    //fixups, seems like writer is doing this... init stuff / rolling code / calibration data / ???
    cpu->eCode[0x03F6] = 0x0100; //RET 0
    cpu->eCode[0x03ED] = 0x0100; //RET 0
    cpu->eCode[0x03EE] = 0x0100; //RET 0
    cpu->eCode[0x03FE] = 0x0100; //RET 0xXY - calibration code is NOT executed / RET 0xFF - calibration code is executed
  }

  pmx150_reset(cpu, true);
}

