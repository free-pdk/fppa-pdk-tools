#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "emucpu.h"

char* disass13(struct emuCPU *cpu, uint16_t position, char *buffer)
{
  char* r=0;
  char tmp[32];
  strcpy( buffer, "?????" );

  uint16_t opcode = emuCPUcodeGet( cpu, position ) & 0x1FFF;

  //special opcode (should be a call, but makes no sense)
  if( 0x1FFF == opcode ) return buffer;

  //13 bit opcodes 0x0000 - 0x003F
  if( opcode<=0x003F )
  {
    switch( opcode )
    {
      case 0x0000: r="NOP"; break;

      case 0x0006: r="LDSPTL"; break;
      case 0x0007: r="LDSPTH"; break;

      case 0x0010: r="ADDC A"; break;
      case 0x0011: r="SUBC A"; break;
      case 0x0012: r="IZSN A"; break;
      case 0x0013: r="DZSN A"; break;

      case 0x0017: r="PCADD A"; break;
      case 0x0018: r="NOT A"; break;
      case 0x0019: r="NEG A"; break;
      case 0x001A: r="SR A"; break;
      case 0x001B: r="SL A"; break;
      case 0x001C: r="SRC A"; break;
      case 0x001D: r="SLC A"; break;
      case 0x001E: r="SWAP A"; break;

      case 0x0020: r="TRAP"; break;

      case 0x0030: r="WDRESET"; break;

      case 0x0032: r="PUSHAF"; break;
      case 0x0033: r="POPAF"; break;

      case 0x0035: r="RESET"; break;
      case 0x0036: r="STOPSYS"; break;
      case 0x0037: r="STOPEXE"; break;
      case 0x0038: r="ENGINT"; break;
      case 0x0039: r="DISGINT"; break;
      case 0x003A: r="RET"; break;
      case 0x003B: r="RETI"; break;
      case 0x003C: r="MUL"; break;
    }
    if(r) 
      sprintf( buffer, "%s", r );
  }
  else
  //8 bit opcodes 0x0060 - 0x00BF
  if( (opcode>=0x0060) && (opcode<=0x00BF) )
  {
    uint8_t addr = opcode&0x1F;
    switch( opcode&0x1FE0 )
    {
      case 0x0060: sprintf(buffer,"XOR IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x0080: sprintf(buffer,"MOV IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x00A0: sprintf(buffer,"MOV A, IO(0x%02X)  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
    }
  }
  else
  //8 bit opcodes 0x00C0 - 0x00FF
  if( (opcode>=0x00C0) && (opcode<=0x00FF) )
  {
    uint8_t addr = opcode&0x1E;
    switch( opcode & 0x1FE1 )
    {
      case 0x00C0: r="STT16 [0x%02X]"; break;
      case 0x00C1: r="LDT16 [0x%02X]"; break;
      case 0x00E0: r="IDXM [[0x%02X]], A"; break;
      case 0x00E1: r="IDXM A, [[0x%02X]]"; break;
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //7 bit opcodes 0x0400 - 0x0BFF
  if( (opcode>=0x0400) && (opcode<=0x0BFF) )
  {
    uint8_t addr = opcode&0x3F;
    switch( opcode & 0x1FC0 )
    {
      case 0x0400: r="ADD [0x%02X], A"; break;
      case 0x0440: r="SUB [0x%02X], A"; break;
      case 0x0480: r="ADDC [0x%02X], A"; break;
      case 0x04C0: r="SUBC [0x%02X], A"; break;
      case 0x0500: r="AND [0x%02X], A"; break;
      case 0x0540: r="OR [0x%02X], A"; break;
      case 0x0580: r="XOR [0x%02X], A"; break;
      case 0x05C0: r="MOV [0x%02X], A"; break;
      case 0x0600: r="ADD A, [0x%02X]"; break;
      case 0x0640: r="SUB A, [0x%02X]"; break;
      case 0x0680: r="ADDC A, [0x%02X]"; break;
      case 0x06C0: r="SUBC A, [0x%02X]"; break;
      case 0x0700: r="AND A, [0x%02X]"; break;
      case 0x0740: r="OR A, [0x%02X]"; break;
      case 0x0780: r="XOR A, [0x%02X]"; break;
      case 0x07C0: r="MOV A, [0x%02X]"; break;
      case 0x0800: r="ADDC [0x%02X]"; break;
      case 0x0840: r="SUBC [0x%02X]"; break;
      case 0x0880: r="IZSN [0x%02X]"; break;
      case 0x08C0: r="DZSN [0x%02X]"; break;
      case 0x0900: r="INCM [0x%02X]"; break;
      case 0x0940: r="DECM [0x%02X]"; break;
      case 0x0980: r="CLEAR [0x%02X]"; break;
      case 0x09C0: r="XCH A, [0x%02X]"; break;
      case 0x0A00: r="NOT A, [0x%02X]"; break;
      case 0x0A40: r="NEG A, [0x%02X]"; break;
      case 0x0A80: r="SR [0x%02X]"; break;
      case 0x0AC0: r="SL [0x%02X]"; break;
      case 0x0B00: r="SRC [0x%02X]"; break;
      case 0x0B40: r="SLC [0x%02X]"; break;
      case 0x0B80: r="CEQSN A, [0x%02X]"; break;
      //case 0x0BC0: r="CNEQSN A, [0x%02X]"; break; //this INS is not implemented
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //5 bit opcodes 0x01.. , 0x1000 - 0x17FF
  if( (0x0100 == (opcode&0x1F00)) || (0x1000 == (opcode&0x1800)) )
  {
    uint8_t k = opcode&0xFF;
    switch( opcode & 0x1F00 )
    {
      case 0x0100: r="RET 0x%02X"; break;
      case 0x1000: r="ADD A, 0x%02X"; break;
      case 0x1100: r="SUB A, 0x%02X"; break;
      case 0x1200: r="CEQSN A, 0x%02X"; break;
      //case 0x1300: r="CNEQSN A, 0x%02X"; break; //this INS is not implemented
      case 0x1400: r="AND A, 0x%02X"; break;
      case 0x1500: r="OR A, 0x%02X"; break;
      case 0x1600: r="XOR A, 0x%02X"; break;
      case 0x1700: r="MOV A, 0x%02X"; break;
    }
    if(r) 
      sprintf( buffer, r, k );
  }
  else
  //5 bit opcodes 0x0200 - 0x03FF
  if( 0x0200 == (opcode&0x1E00) )
  {
    uint8_t bit = (opcode>>5)&7;
    uint8_t addr = opcode&0x1E;
    switch( opcode & 0x1F01 )
    {
      case 0x0200: sprintf(buffer,"T0SN [0x%02X].%d", addr,bit); break;
      case 0x0201: sprintf(buffer,"T1SN [0x%02X].%d", addr,bit); break;
      case 0x0300: sprintf(buffer,"SET0 [0x%02X].%d", addr,bit); break;
      case 0x0301: sprintf(buffer,"SET1 [0x%02X].%d", addr,bit); break;
    }
  }
  else
  //5 bit opcodes 0x0C00 - 0x0FFF
  if( (opcode>=0x0C00) && (opcode<=0x0FFF) )
  {
    uint8_t bit = (opcode>>5)&7;
    uint8_t addr = opcode&0x1F;
    switch( opcode & 0x1F00 )
    {
      case 0x0C00: sprintf(buffer,"T0SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x0D00: sprintf(buffer,"T1SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x0E00: sprintf(buffer,"SET0 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x0F00: sprintf(buffer,"SET1 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
    }
  }
  else
  //3 bit opcodes 0x1800 - 0x1FFF
  if( (0x1800 == (opcode&0x1800)) )
  {
    if( opcode & 0x0400 )
      sprintf(buffer,"CALL 0x%03X", opcode & 0x03FF); 
    else
      sprintf(buffer,"GOTO 0x%03X", opcode & 0x03FF); 
  }

  return buffer;
}
