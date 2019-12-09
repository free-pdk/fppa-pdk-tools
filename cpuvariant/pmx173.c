#include <string.h>
#include <stdio.h>
#include "cpuvariant.h"
#include "emucpu.h"
#include "opcode.h"
#include "disass.h"

void pmx173_reset(struct emuCPU *cpu, bool clearRAM)
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

char* pmx173_ioname(struct emuCPU *cpu, uint8_t io, uint8_t bit, char* buffer)
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
        case 0: r="INTEN PA0/PB5"; break;
        case 1: r="INTEN PB0/PA4"; break;
        case 2: r="INTEN T16"; break;
        case 3: r="INTEN ADC"; break;
        case 4: r="INTEN COMP"; break;
        case 5: r="INTEN PWMG"; break;
        case 6: r="INTEN TM2"; break;
        case 7: r="INTEN TM3"; break;
        default: r="INTEN";
      }
      break;

    case 0x05: 
      switch( bit )
      {
        case 0: r="INTRQ PA0/PB5"; break;
        case 1: r="INTRQ PB0/PA4"; break;
        case 2: r="INTRQ T16"; break;
        case 3: r="INTRQ ADC"; break;
        case 4: r="INTRQ COMP"; break;
        case 5: r="INTRQ PWMG"; break;
        case 6: r="INTRQ TM2"; break;
        case 7: r="INTRQ TM3"; break;
        default: r="INTRQ";
      }
      break;

    case 0x06: r="T16M"; break;

    case 0x0A: r="EOSCR"; break;
    case 0x0B: r="IHRCR"; break;
    case 0x0C: r="INTEGS"; break;
    case 0x0D: if(bit<8) sprintf(buffer,"PADIER.%d",bit); else r="PADIER";  break;
    case 0x0E: if(bit<8) sprintf(buffer,"PBDIER.%d",bit); else r="PBDIER";  break;
    case 0x0F: if(bit<8) sprintf(buffer,"PCDIER.%d",bit); else r="PCDIER";  break;
    case 0x10: if(bit<8) sprintf(buffer,"PA.%d",bit);  else r="PA";  break;
    case 0x11: if(bit<8) sprintf(buffer,"PAC.%d",bit);  else r="PAC";  break;
    case 0x12: if(bit<8) sprintf(buffer,"PAPH.%d",bit);  else r="PAPH";  break;
    case 0x13: if(bit<8) sprintf(buffer,"PB.%d",bit);  else  r="PB";  break;
    case 0x14: if(bit<8) sprintf(buffer,"PBC.%d",bit);  else  r="PBC";  break;
    case 0x15: if(bit<8) sprintf(buffer,"PBPH.%d",bit);  else  r="PBPH";  break;
    case 0x16: if(bit<8) sprintf(buffer,"PC.%d",bit);  else  r="PC";  break;
    case 0x17: if(bit<8) sprintf(buffer,"PCC.%d",bit);  else  r="PCC";  break;
    case 0x18: if(bit<8) sprintf(buffer,"PCPH.%d",bit);  else  r="PCPH";  break;
    case 0x19: if(bit<8) sprintf(buffer,"PBPL.%d",bit);  else  r="PBPL";  break;
    case 0x1A: if(bit<8) sprintf(buffer,"PCPL.%d",bit);  else  r="PCPL";  break;

    case 0x20: r="ADCC"; break;
    case 0x21: r="ADCM"; break;
    case 0x22: r="ADCR"; break;

    case 0x24: r="ADCRGC"; break;

    case 0x26: r="MISC"; break;
    case 0x27: r="MISC2"; break;
    case 0x28: r="MISCLVR"; break;

    case 0x2B: r="GPCC"; break;
    case 0x2C: r="GPCS"; break;

    case 0x30: r="TM2C"; break;
    case 0x31: r="TM2CT"; break;
    case 0x32: r="TM2S"; break;
    case 0x33: r="TM2B"; break;
    case 0x34: r="TM3C"; break;
    case 0x35: r="TM3CT"; break;
    case 0x36: r="TM3S"; break;
    case 0x37: r="TM3B"; break;

    case 0x40: r="PWMG0C"; break;
    case 0x41: r="PWMGCLK"; break;
    case 0x42: r="PWMG0DTH"; break;
    case 0x43: r="PWMG0DTL"; break;
    case 0x44: r="PWMG0CUBH"; break;
    case 0x45: r="PWMG0CUBL"; break;
    case 0x46: r="PWMG1C"; break;

    case 0x48: r="PWMG1DTH"; break;
    case 0x49: r="PWMG1DTL"; break;

    case 0x4C: r="PWMG2C"; break;

    case 0x4E: r="PWMG2DTH"; break;
    case 0x4F: r="PWMG2DTL"; break;

    case 0x62: r="ILRCR"; break;
    case 0x63: r="BGTR"; break;

    case 0x67: r="ROP"; break;

    default:
      r="?";
  }

  if( r )
    strcpy(buffer,r);

  return buffer;
}

void pmx173_peripherals(struct emuCPU *cpu)
{
//TODO: INTERRUPT EMU (PC=>[SP];SP+=2;}
//TODO: TIMER EMU
//TODO: WDCOUNTER, ...
//TODO: PWM EMU
//TODO: CLK EMU
//TODO: PA / PB EMU
//TODO: IO 0x3F ==> virtual putchar for easy emu
}

void pmx173_init(struct emuCPU *cpu, bool fixupHighCode)
{
  cpu->maxIO = 64;
  cpu->maxMem = 256;
  cpu->maxCode = 3096;

  cpu->fnReset = pmx173_reset;
  cpu->fnExecute = opcode15;
  cpu->fnDisassemble = disass15;
  cpu->fnPeripherals = pmx173_peripherals;
  cpu->fnIOName = pmx173_ioname;

  if( fixupHighCode )
  {
    //fixups, seems like writer is doing this... init stuff / rolling code / calibration data / ???
    cpu->eCode[0x0BF6] = 0x0200; //RET 0
    cpu->eCode[0x0BFE] = 0x0200; //RET 0xXY - calibration code is NOT executed / RET 0xFF - calibration code is executed
  }

  cpu->eCode[0x0BED] = 0x027F; //IHRCR factory calibration
  cpu->eCode[0x0BEE] = 0x026A; //BGTR factory calibration

  pmx173_reset(cpu, true);
}
