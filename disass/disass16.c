#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "emucpu.h"

char* disass16(struct emuCPU *cpu, uint16_t position, char *buffer)
{
  char* r=0;
  char tmp[32];
  strcpy( buffer, "?????" );

  uint16_t opcode = emuCPUcodeGet( cpu, position );

  //special opcode (should be a call, but makes no sense)
  if( 0xFFFF == opcode ) return buffer;

  //16 bit opcodes 0x0000 - 0x003F
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
      case 0x001F: r="DELAY A"; break;

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
  //11 bit opcode 0x0040 - 0x005F
  if( 0x0040 == (opcode&0xFFE0) )
  {
    sprintf(buffer,"PMODE %d",opcode&0x1F);
  }
  //12 bit opcode 0x0060 - 0x007F
  if( 0x0060 == (opcode&0xFFE0) )
  {
    switch( opcode&0xFFF0 )
    {
      case 0x0060: sprintf(buffer,"POPW PC %d", opcode&0xF); break;
      case 0x0070: sprintf(buffer,"PUSHW PC %d", opcode&0xF); break;
    }
  }
  //10 bit opcodes 0x0080 - 0x00FF, 0x1000 - 0x1070
  if( (0x0080 == (opcode&0xFF80)) || (0x1000 == (opcode&0xFF80)) )
  {
    uint8_t addr = opcode&0x3F;
    switch( opcode&0xFFC0 )
    {
      case 0x0080: sprintf(buffer,"MOV IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x00C0: sprintf(buffer,"MOV A, IO(0x%02X)  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x1000: sprintf(buffer,"XOR IO(0x%02X), A  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
      case 0x1040: sprintf(buffer,"XOR A, IO(0x%02X)  ;%s", addr, emuCPUdecodeIO(cpu,addr,0xFF,tmp)); break;
    }
  }
  else
  //7 bit opcodes 0x0200 - 0x0BFF
  if( (opcode>=0x0200) && (opcode<=0x0BFF) )
  {
    uint16_t addr = opcode&0x1FE;
    switch( opcode & 0xFE01 )
    {
      case 0x0200: r="STT16 [0x%03X]"; break;
      case 0x0201: r="LDT16 [0x%03X]"; break;
      case 0x0400: r="POPW [0x%03X]"; break;
      case 0x0401: r="PUSHW [0x%03X]"; break;
      case 0x0600: r="IGOTO [0x%03X]"; break;
      case 0x0601: r="ICALL [0x%03X]"; break;
      case 0x0800: r="IDXM [[0x%03X]], A"; break;
      case 0x0801: r="IDXM A, [[0x%03X]]"; break;
      case 0x0A00: r="LDTABL [0x%03X]"; break;
      case 0x0A01: r="LDTABH [0x%03X]"; break;
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //7 bit opcodes 0x1400 - 0x17FF,  0x3000 - 0x7FFF
  if( ((opcode>=0x1400) && (opcode<=0x17FF)) ||
      ((opcode>=0x3000) && (opcode<=0x7FFF)) )
  {
    int16_t addr = opcode&0x1FF;
    switch( opcode & 0xFFE0 )
    {
      case 0x1400: r="CNEQSN [0x%03X], A"; break;
      case 0x1600: r="CNEQSN A, [0x%03X]"; break;
      case 0x3000: r="NMOV [0x%03X], A"; break;
      case 0x3200: r="NMOV A, [0x%03X]"; break;
      case 0x3400: r="NADD [0x%03X], A"; break;
      case 0x3600: r="NADD A, [0x%03X]"; break;
      case 0x3800: r="CEQSN [0x%03X], A"; break;
      case 0x3A00: r="CEQSN A, [0x%03X]"; break;
      case 0x3C00: r="COMP [0x%03X], A"; break;
      case 0x3E00: r="COMP A, [0x%03X]"; break;
      case 0x4000: r="ADD [0x%03X], A"; break;
      case 0x4200: r="ADD A, [0x%03X]"; break;
      case 0x4400: r="SUB [0x%03X], A"; break;
      case 0x4600: r="SUB A, [0x%03X]"; break;
      case 0x4800: r="ADDC [0x%03X], A"; break;
      case 0x4A00: r="ADDC A, [0x%03X]"; break;
      case 0x4C00: r="SUBC [0x%03X], A"; break;
      case 0x4E00: r="SUBC A, [0x%03X]"; break;
      case 0x5000: r="AND [0x%03X], A"; break;
      case 0x5200: r="AND A, [0x%03X]"; break;
      case 0x5400: r="OR [0x%03X], A"; break;
      case 0x5600: r="OR A, [0x%03X]"; break;
      case 0x5800: r="XOR [0x%03X], A"; break;
      case 0x5A00: r="XOR A, [0x%03X]"; break;
      case 0x5C00: r="MOV [0x%03X], A"; break;
      case 0x5E00: r="MOV A, [0x%03X]"; break;
      case 0x6000: r="ADDC [0x%03X]"; break;
      case 0x6200: r="SUBC [0x%03X]"; break;
      case 0x6400: r="IZSN [0x%03X]"; break;
      case 0x6600: r="DZSN [0x%03X]"; break;
      case 0x6800: r="INCM [0x%03X]"; break;
      case 0x6A00: r="DECM [0x%03X]"; break;
      case 0x6C00: r="CLEAR [0x%03X]"; break;
      case 0x6E00: r="XCH A, [0x%03X]"; break;
      case 0x7000: r="NOT A, [0x%03X]"; break;
      case 0x7200: r="NEG A, [0x%03X]"; break;
      case 0x7400: r="SR [0x%03X]"; break;
      case 0x7600: r="SL [0x%03X]"; break;
      case 0x7800: r="SRC [0x%03X]"; break;
      case 0x7A00: r="SLC [0x%03X]"; break;
      case 0x7C00: r="SWAP [0x%03X]"; break;
      case 0x7E00: r="DELAY [0x%03X]"; break;
    }
    if(r) 
      sprintf( buffer, r, addr );
  }
  else
  //8 bit opcodes 0x0E00 - 0x0FFF, 0x1800 - 0x1FFF
  if( (0x0E00 == (opcode&0xFE00)) || (0x1800 == (opcode&0xF800)) )
  {
    uint8_t k = opcode&0xFF;
    switch( opcode & 0xFF00 )
    {
      case 0x0E00: r="DELAY 0x%02X"; break;
      case 0x0F00: r="RET 0x%02X"; break;
      case 0x1800: r="ADD A, 0x%02X"; break;
      case 0x1900: r="SUB A, 0x%02X"; break;
      case 0x1A00: r="CEQSN A, 0x%02X"; break;
      case 0x1B00: r="CNEQSN A, 0x%02X"; break;
      case 0x1C00: r="AND A, 0x%02X"; break;
      case 0x1D00: r="OR A, 0x%02X"; break;
      case 0x1E00: r="XOR A, 0x%02X"; break;
      case 0x1F00: r="MOV A, 0x%02X"; break;
    }
    if(r)
      sprintf( buffer, r, k );
  }
  else
  //7 bit opcodes 0x2000 - 0x2FFF
  if( 0x2000 == (opcode&0xF000) )
  {
    uint8_t bit = (opcode>>6)&7;
    uint8_t addr = opcode&0x3F;
    switch( opcode & 0xFE00 )
    {
      case 0x2000: sprintf(buffer,"T0SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2200: sprintf(buffer,"T1SN IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2400: sprintf(buffer,"SET0 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2600: sprintf(buffer,"SET1 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2800: sprintf(buffer,"TOG IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2A00: sprintf(buffer,"WAIT0 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2C00: sprintf(buffer,"WAIT1 IO(0x%02X).%d  ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
      case 0x2E00: sprintf(buffer,"SWAPC IO(0x%02X).%d ;%s", addr,bit,emuCPUdecodeIO(cpu,addr,bit,tmp)); break;
    }
  }
  else
  //4 bit opcodes 0x8000 - 0xBFFF
  if( (opcode>=0x8000) && (opcode<=0xBFFF) )
  {
    uint8_t bit = (opcode>>9)&7;
    uint16_t addr = opcode&0x1FF;
    switch( opcode & 0xF000 )
    {
      case 0x8000: sprintf(buffer,"T0SN [0x%03X].%d", addr,bit); break;
      case 0x9000: sprintf(buffer,"T1SN [0x%03X].%d", addr,bit); break;
      case 0xA000: sprintf(buffer,"SET0 [0x%03X].%d", addr,bit); break;
      case 0xB000: sprintf(buffer,"SET1 [0x%03X].%d", addr,bit); break;
    }
  }
  else
  //3 bit opcodes 0xC000 - 0xFFFF
  if( (0xC000 == (opcode&0xC000)) )
  {
    if( opcode & 0x4000 )
      sprintf(buffer,"CALL 0x%03X", opcode & 0x1FFF);
    else
      sprintf(buffer,"GOTO 0x%03X", opcode & 0x1FFF);
  }

  return buffer;
}
