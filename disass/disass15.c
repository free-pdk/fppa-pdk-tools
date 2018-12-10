#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "emucpu.h"

char* disass15(struct emuCPU *cpu, uint16_t position, char *buffer)
{
  char* r=0;
  char tmp[32];
  strcpy( buffer, "?????" );

  uint16_t opcode = emuCPUcodeGet( cpu, position ) & 0x7FFF;

  //special opcode (should be a call, but makes no sense)
  if( 0x7FFF == opcode ) return buffer;

  //15 bit opcodes 0x0000 - 0x007F
  if( opcode<=0x00BF )
  {
    switch( opcode )
    {
      case 0x0000: r="NOP"; break;

      case 0x0006: r="LDSPTL"; break;
      case 0x0007: r="LDSPTH"; break;

      case 0x0060: r="ADDC A"; break;
      case 0x0061: r="SUBC A"; break;
      case 0x0062: r="IZSN A"; break;
      case 0x0063: r="DZSN A"; break;

      case 0x0067: r="PCADD A"; break;
      case 0x0068: r="NOT A"; break;
      case 0x0069: r="NEG A"; break;
      case 0x006A: r="SR A"; break;
      case 0x006B: r="SL A"; break;
      case 0x006C: r="SRC A"; break;
      case 0x006D: r="SLC A"; break;
      case 0x006E: r="SWAP A"; break;

      case 0x0070: r="WDRESET"; break;

      case 0x0072: r="PUSHAF"; break;
      case 0x0073: r="POPAF"; break;

      case 0x0075: r="RESET"; break;
      case 0x0076: r="STOPSYS"; break;
      case 0x0077: r="STOPEXE"; break;
      case 0x0078: r="ENGINT"; break;
      case 0x0079: r="DISGINT"; break;
      case 0x007A: r="RET"; break;
      case 0x007B: r="RETI"; break;
      case 0x007C: r="MUL"; break;
    }
    if(r) 
      sprintf( buffer, "%s", r );
  }
  else
  //9 bit opcodes 0x0080 - 0x01FF
  if( (opcode>=0x00C0) && (opcode<=0x01FF) )
  {
    uint8_t addr = opcode&0x3F;
    switch( opcode&0x7FC0 )
    {
      case 0x0080: sprintf(buffer,"XOR IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x0100: sprintf(buffer,"MOV IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x0180: sprintf(buffer,"MOV A, IO(0x%02X)  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
    }
  }
  else
  //7 bit opcodes 0x0500-0x7FF
  if( (opcode>=0x0500) && (opcode<=0x07FF) )
  {
    uint8_t addr = opcode&0xFE;
    switch( opcode & 0x7F01 )
    {
      case 0x0500: r="LDTABL [0x%02X]"; break;
      case 0x0501: r="LDTABH [0x%02X]"; break;
      case 0x0600: r="STT16 [0x%02X]"; break;
      case 0x0601: r="LDT16 [0x%02X]"; break;
      case 0x0700: r="IDXM [[0x%02X]], A"; break;
      case 0x0701: r="IDXM A, [[0x%02X]]"; break;
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //7 bit opcodes 0x0C00 - 0x2FFF
  if( (opcode>=0x0C00) && (opcode<=0x2FFF) )
  {
    int8_t addr = opcode&0xFF;
    switch( opcode & 0x7F00 )
    {
      case 0x0C00: r="COMP A, [0x%02X]"; break;
      case 0x0D00: r="COMP [0x%02X], A"; break;
      case 0x0E00: r="NADD A, [0x%02X]"; break;
      case 0x0F00: r="NADD [0x%02X], A"; break;
      case 0x1000: r="ADD [0x%02X], A"; break;
      case 0x1100: r="SUB [0x%02X], A"; break;
      case 0x1200: r="ADDC [0x%02X], A"; break;
      case 0x1300: r="SUBC [0x%02X], A"; break;
      case 0x1400: r="AND [0x%02X], A"; break;
      case 0x1500: r="OR [0x%02X], A"; break;
      case 0x1600: r="XOR [0x%02X], A"; break;
      case 0x1700: r="MOV [0x%02X], A"; break;
      case 0x1800: r="ADD A, [0x%02X]"; break;
      case 0x1900: r="SUB A, [0x%02X]"; break;
      case 0x1A00: r="ADDC A, [0x%02X]"; break;
      case 0x1B00: r="SUBC A, [0x%02X]"; break;
      case 0x1C00: r="AND A, [0x%02X]"; break;
      case 0x1D00: r="OR A, [0x%02X]"; break;
      case 0x1E00: r="XOR A, [0x%02X]"; break;
      case 0x1F00: r="MOV A, [0x%02X]"; break;
      case 0x2000: r="ADDC [0x%02X]"; break;
      case 0x2100: r="SUBC [0x%02X]"; break;
      case 0x2200: r="IZSN [0x%02X]"; break;
      case 0x2300: r="DZSN [0x%02X]"; break;
      case 0x2400: r="INCM [0x%02X]"; break;
      case 0x2500: r="DECM [0x%02X]"; break;
      case 0x2600: r="CLEAR [0x%02X]"; break;
      case 0x2700: r="XCH A, [0x%02X]"; break;
      case 0x2800: r="NOT A, [0x%02X]"; break;
      case 0x2900: r="NEG A, [0x%02X]"; break;
      case 0x2A00: r="SR [0x%02X]"; break;
      case 0x2B00: r="SL [0x%02X]"; break;
      case 0x2C00: r="SRC [0x%02X]"; break;
      case 0x2D00: r="SLC [0x%02X]"; break;
      case 0x2E00: r="CEQSN A, [0x%02X]"; break;
      case 0x2F00: r="CNEQSN A, [0x%02X]"; break;
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //7 bit opcodes 0x02.. , 0x5000 - 0x57FF
  if( (0x0200 == (opcode&0x7F00)) || (0x5000 == (opcode&0x7800)) )
  {
    uint8_t k = opcode&0xFF;
    switch( opcode & 0x7F00 )
    {
      case 0x0200: r="RET 0x%02X"; break;
      case 0x5000: r="ADD A, 0x%02X"; break;
      case 0x5100: r="SUB A, 0x%02X"; break;
      case 0x5200: r="CEQSN A, 0x%02X"; break;
      case 0x5300: r="CNEQSN A, 0x%02X"; break;
      case 0x5400: r="AND A, 0x%02X"; break;
      case 0x5500: r="OR A, 0x%02X"; break;
      case 0x5600: r="XOR A, 0x%02X"; break;
      case 0x5700: r="MOV A, 0x%02X"; break;
    }
    if(r) 
      sprintf( buffer, r, k );
  }                
  else
  //5 bit opcodes 0x3000 - 0x4FFF, 0x5C00 - 0x5FFF
  if( ((opcode>=0x3000) && (opcode<=0x4FFF)) || (0x5C00 == (opcode&0x7F00)) )
  {
    uint8_t bit = (opcode>>7)&7;
    uint8_t addr = opcode&0x7F;
    switch( opcode & 0x7C00 )
    {
      case 0x3000: sprintf(buffer,"T0SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x3400: sprintf(buffer,"T1SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x3800: sprintf(buffer,"SET0 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x3C00: sprintf(buffer,"SET1 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x4000: sprintf(buffer,"T0SN [0x%02X].%d", addr,bit); break;
      case 0x4400: sprintf(buffer,"T1SN [0x%02X].%d", addr,bit); break;
      case 0x4800: sprintf(buffer,"SET0 [0x%02X].%d", addr,bit); break;
      case 0x4C00: sprintf(buffer,"SET1 [0x%02X].%d", addr,bit); break;
      case 0x5C00: sprintf(buffer,"SWAPC IO(0x%02X).%d ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
    }
  }
  else
  //3 bit opcodes 0x6000 - 0x7FFF
  if( (0x6000 == (opcode&0x6000)) )
  {
    if( opcode & 0x1000 )
      sprintf(buffer,"CALL 0x%03X", opcode & 0x0FFF); 
    else
      sprintf(buffer,"GOTO 0x%03X", opcode & 0x0FFF); 
  }

  return buffer;
}
