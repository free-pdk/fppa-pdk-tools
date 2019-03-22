#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "emucpu.h"
#include "pdkformat.h"
#include "cpuvariant.h"

int emuCPUinit(struct emuCPU *cpu, uint8_t* hdr, uint32_t hdrlen, bool fixupHighCode)
{
  cpu->maxIO = 0;
  cpu->maxMem = 0;
  cpu->maxCode = 0;

  cpu->fnReset = 0;
  cpu->fnExecute = 0;
  cpu->fnPeripherals = 0;
  cpu->fnException = 0;
  cpu->fnIORead = 0;
  cpu->fnIOWrite = 0;
  cpu->fnIOName = 0;

  if( hdrlen>sizeof(cpu->hdr) )
    return -1;
  if( hdr && hdrlen)
    memcpy( &cpu->hdr, hdr, hdrlen );
  cpu->hdrlen = hdrlen;

  switch( cpu->hdr.otp_id )
  {
    case 0x0B80: // PMS150 / PMC150
    case 0x1E01: // PMS150B
    case 0x2A16: // PMS150C
      pmx150_init(cpu,fixupHighCode); break;

    case 0x2A06: // PMS154
    case 0x2C06: // PMS154B / PMS154C
    case 0x2AA1: // PFS154 //TODO: much different ? own impl?
    case 0x2AA4: // PFC154 //TODO: much different ? own impl?
      pmx154_init(cpu,fixupHighCode); break;

    case 0x2AA2: // PFS173
      pmx173_init(cpu,fixupHighCode); break;

    default:
      if( 0 != generic_init(cpu) )
        return -3;
  }

  if( !cpu->fnReset )
    return -2;

  return 0;
}

int emuCPUloadPDK(struct emuCPU *cpu, const char *filename, bool fixupHighCode)
{
  memset( cpu, 0, sizeof(struct emuCPU) );

  FILE* fin = fopen( filename, "rb");
  if( !fin )
    return -1; //could not open file

  uint8_t pdk[10000];
  int pdklen =fread( pdk, 1, sizeof(pdk), fin );
  if( pdklen<=0 ) 
    return -2; //error reading input file
  fclose(fin);

  int32_t hdrlen = pdkhdrlen(pdk, pdklen);
  if( hdrlen<0 )
    return -3;

  if( emuCPUinit(cpu, pdk, hdrlen, fixupHighCode) < 0 )
    return -4; //no emulator found for cpu type

  if( (pdklen-hdrlen)>(cpu->maxCode*sizeof(uint16_t)) )
    return -5; //code size to big

  memset( cpu->eCode, 0xFF, cpu->maxMem );
  if( depdk( pdk, pdklen, (uint8_t*)cpu->eCode, cpu->maxCode*sizeof(uint16_t) ) < 0 )
    return -6; //error decrypting input file

  cpu->fnReset( cpu, true );

  return 0;
}

int emuCPUloadBIN(struct emuCPU *cpu, const char *filename, bool fixupHighCode, uint16_t otp_id)
{
  FILE* fin = fopen( filename, "rb");
  if( !fin ) 
    return -1; //could not open file

  uint8_t bin[10000];
  int binlen =fread( bin, 1, sizeof(bin), fin );
  if( binlen<=0 ) 
    return -2; //error reading input file
  fclose(fin);

  //manually setup minimal cpu header
  memset( cpu, 0, sizeof(struct emuCPU) );
  cpu->hdrlen = sizeof(cpu->hdr);
  cpu->hdr.otp_id = otp_id;

  if( emuCPUinit(cpu, 0, 0, fixupHighCode) < 0 )
    return -4; //no emulator found for cpu type

  if( binlen>(cpu->maxCode*sizeof(uint16_t)) )
    return -5; //code size to big

  memset( cpu->eCode, 0xFF, cpu->maxMem );

  memcpy( cpu->eCode, bin, binlen );
  cpu->hdr.codesize = binlen/sizeof(uint16_t);

  cpu->fnReset( cpu, true );

  return 0;
}



void emuCPUexception(struct emuCPU *cpu, int code)
{
  if( cpu->fnException )
    cpu->fnException( cpu, code );
}

uint8_t emuCPUioGet(struct emuCPU *cpu, uint8_t addr)
{
  if( addr<cpu->maxIO )
  {
    if( cpu->fnIORead )
      return cpu->fnIORead(cpu, addr);
    else
      return cpu->eIO[addr];
  }

  emuCPUexception(cpu, EXCEPTION_IO); //invalid io read
  return 0xFF;
}

void emuCPUioPut(struct emuCPU *cpu, uint8_t addr, uint8_t dat)
{
  if( addr<cpu->maxIO )
  {
    cpu->eIO[addr] = dat;
    if( cpu->fnIOWrite )
      cpu->fnIOWrite( cpu, addr );
  }
  else
    emuCPUexception(cpu, EXCEPTION_IO); //invalid io write
}

char* emuCPUdecodeIO(struct emuCPU *cpu, uint8_t addr, uint8_t bit, char* buffer)
{
  if( cpu->fnIOName )
    return cpu->fnIOName(cpu,addr,bit,buffer);

  *buffer = 0;
  return buffer;
}



uint8_t emuCPUmemGet(struct emuCPU *cpu, uint8_t addr)
{
  if( addr<cpu->maxMem )
    return cpu->eMem[addr];

  emuCPUexception(cpu, EXCEPTION_MEMORY); //invalid memory read
  return 0xFF;
}

void emuCPUmemPut(struct emuCPU *cpu, uint8_t addr, uint8_t dat)
{
  if( addr<cpu->maxMem )
    cpu->eMem[addr] = dat;
  else
    emuCPUexception(cpu, EXCEPTION_MEMORY); //invalid memory write
}

uint16_t emuCPUcodeGet(struct emuCPU *cpu, uint16_t addr)
{
  if( addr<cpu->maxCode )
    return cpu->eCode[addr];

  emuCPUexception(cpu, EXCEPTION_CODE); //invalid code read
  return 0xFFFF;
}

void emuCPUstackPush(struct emuCPU *cpu, uint8_t dat)
{
  emuCPUmemPut( cpu, eSP++, dat );
}

uint8_t emuCPUstackPop(struct emuCPU *cpu)
{
  return emuCPUmemGet( cpu, --eSP );
}

void emuCPUstackPushW(struct emuCPU *cpu, uint16_t dat)
{
  emuCPUstackPush(cpu,dat);
  emuCPUstackPush(cpu,dat>>8);
}

uint16_t emuCPUstackPopW(struct emuCPU *cpu)
{
  uint16_t dat;
  dat  = emuCPUstackPop(cpu);
  dat <<= 8;
  dat |= emuCPUstackPop(cpu);
  return dat;
}

uint8_t emuCPUaddSolveFlagsVACZ(int16_t value1, int16_t value2, int16_t c)
{
  int zr = !(((value1&0xFF)+(value2&0xFF)+c)&0xFF);
  int cy = (((value1&0xFF)+(value2&0xFF)+c)>>8)&1;
  int ac = (((value1&0xF)+(value2&0xF)+c)>>4)&1;
  int ov = ((((value1&0x7F)+(value2&0x7F)+c)>>7)&1)^cy;
  return( (ov<<3)|(ac<<2)|(cy<<1)|zr );
}

uint8_t emuCPUsubSolveFlagsVACZ(int16_t value1, int16_t value2, int16_t c)
{
  int zr = !(((value1&0xFF)-(value2&0xFF)-c)&0xFF);
  int cy = (((value1&0xFF)-(value2&0xFF)-c)>>8)&1;
  int ac = ((value1&0xF)<((value2&0xF)+c))?1:0;
  int ov = ((((value1&0x7F)-(value2&0x7F))>>7)&1)^cy;
  return( (ov<<3)|(ac<<2)|(cy<<1)|zr );
}
