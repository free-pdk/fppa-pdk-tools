#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "emucpu.h"

char* disass14(struct emuCPU *cpu, uint16_t position, char *buffer)
{
  char* r=0;
  char tmp[32];
  strcpy( buffer, "?????" );

  uint16_t opcode = emuCPUcodeGet( cpu, position ) & 0x3FFF;

  //special opcode (should be a call, but makes no sense)
  if( 0x3FFF == opcode ) return buffer;

  //14 bit opcodes 0x0000 - 0x00BF
  if( opcode<=0x00BF )
  {
    switch( opcode )
    {
      case 0x0000: r="NOP"; break;

      case 0x0006: r="LDSPTL"; break;
      case 0x0007: r="LDSPTH"; break;

      case 0x0020: r="TRAP"; break;

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
  //8 bit opcodes 0x00C0 - 0x01FF
  if( (opcode>=0x00C0) && (opcode<=0x01FF) )
  {
    uint8_t addr = opcode&0x3F;
    switch( opcode&0x3FC0 )
    {
      case 0x00C0: sprintf(buffer,"XOR IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x0180: sprintf(buffer,"MOV IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x01C0: sprintf(buffer,"MOV A, IO(0x%02X)  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
    }
  }
  else
  //7 bit opcodes 0x03..
  if( 0x0300 == (opcode&0x3F00) )
  {
    uint8_t addr = opcode&0x7E;
    switch( opcode & 0x3F81 )
    {
      case 0x0300: r="STT16 [0x%02X]"; break;
      case 0x0301: r="LDT16 [0x%02X]"; break;
      case 0x0380: r="IDXM [[0x%02X]], A"; break;
      case 0x0381: r="IDXM A, [[0x%02X]]"; break;
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //7 bit opcodes 0x0600 - 0x17FF
  if( (opcode>=0x0600) && (opcode<=0x17FF) )
  {
    uint8_t addr = opcode&0x7F;
    switch( opcode & 0x3F80 )
    {
      case 0x0600: r="COMP A, [0x%02X]"; break;
      case 0x0680: r="COMP [0x%02X], A"; break;
      case 0x0700: r="NADD A, [0x%02X]"; break;
      case 0x0780: r="NADD [0x%02X], A"; break;
      case 0x0800: r="ADD [0x%02X], A"; break;
      case 0x0880: r="SUB [0x%02X], A"; break;
      case 0x0900: r="ADDC [0x%02X], A"; break;
      case 0x0980: r="SUBC [0x%02X], A"; break;
      case 0x0A00: r="AND [0x%02X], A"; break;
      case 0x0A80: r="OR [0x%02X], A"; break;
      case 0x0B00: r="XOR [0x%02X], A"; break;
      case 0x0B80: r="MOV [0x%02X], A"; break;
      case 0x0C00: r="ADD A, [0x%02X]"; break;
      case 0x0C80: r="SUB A, [0x%02X]"; break;
      case 0x0D00: r="ADDC A, [0x%02X]"; break;
      case 0x0D80: r="SUBC A, [0x%02X]"; break;
      case 0x0E00: r="AND A, [0x%02X]"; break;
      case 0x0E80: r="OR A, [0x%02X]"; break;
      case 0x0F00: r="XOR A, [0x%02X]"; break;
      case 0x0F80: r="MOV A, [0x%02X]"; break;
      case 0x1000: r="ADDC [0x%02X]"; break;
      case 0x1080: r="SUBC [0x%02X]"; break;
      case 0x1100: r="IZSN [0x%02X]"; break;
      case 0x1180: r="DZSN [0x%02X]"; break;
      case 0x1200: r="INCM [0x%02X]"; break;
      case 0x1280: r="DECM [0x%02X]"; break;
      case 0x1300: r="CLEAR [0x%02X]"; break;
      case 0x1380: r="XCH A, [0x%02X]"; break;
      case 0x1400: r="NOT A, [0x%02X]"; break;
      case 0x1480: r="NEG A, [0x%02X]"; break;
      case 0x1500: r="SR [0x%02X]"; break;
      case 0x1580: r="SL [0x%02X]"; break;
      case 0x1600: r="SRC [0x%02X]"; break;
      case 0x1680: r="SLC [0x%02X]"; break;
      case 0x1700: r="CEQSN A, [0x%02X]"; break;
      case 0x1780: r="CNEQSN A, [0x%02X]"; break;
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //6 bit opcodes 0x02.. , 0x2800 - 0x2FFF
  if( (0x0200 == (opcode&0x3F00)) || (0x2800 == (opcode&0x3800)) )
  {
    uint8_t k = opcode&0xFF;
    switch( opcode & 0x3F00 )
    {
      case 0x0200: r="RET 0x%02X"; break;
      case 0x2800: r="ADD A, 0x%02X"; break;
      case 0x2900: r="SUB A, 0x%02X"; break;
      case 0x2A00: r="CEQSN A, 0x%02X"; break;
      case 0x2B00: r="CNEQSN A, 0x%02X"; break;
      case 0x2C00: r="AND A, 0x%02X"; break;
      case 0x2D00: r="OR A, 0x%02X"; break;
      case 0x2E00: r="XOR A, 0x%02X"; break;
      case 0x2F00: r="MOV A, 0x%02X"; break;
    }
    if(r) 
      sprintf( buffer, r, k );
  }                
  else
  //5 bit opcodes 0x0400 - 0x0500, 0x1800 - 0x27FF
  if( (0x0400 == (opcode&0x3E00)) || ((opcode>=0x1800) && (opcode<=0x27FF)) )
  {
    uint8_t bit = (opcode>>6)&7;
    uint8_t addr = opcode&0x3F;
    switch( opcode & 0x3E00 )
    {
      case 0x0400: sprintf(buffer,"SWAPC IO(0x%02X).%d ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x1800: sprintf(buffer,"T0SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x1A00: sprintf(buffer,"T1SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x1C00: sprintf(buffer,"SET0 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x1E00: sprintf(buffer,"SET1 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2000: sprintf(buffer,"T0SN [0x%02X].%d", addr,bit); break;
      case 0x2200: sprintf(buffer,"T1SN [0x%02X].%d", addr,bit); break;
      case 0x2400: sprintf(buffer,"SET0 [0x%02X].%d", addr,bit); break;
      case 0x2600: sprintf(buffer,"SET1 [0x%02X].%d", addr,bit); break;
    }
  }
  else
  //3 bit opcodes 0x3000 - 0x3FFF
  if( (0x3000 == (opcode&0x3000)) )
  {
    if( opcode & 0x0800 )
      sprintf(buffer,"CALL 0x%03X", opcode & 0x07FF); 
    else
      sprintf(buffer,"GOTO 0x%03X", opcode & 0x07FF); 
  }

  return buffer;
}
