#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "emucpu.h"

void emuException(struct emuCPU *cpu, int code)
{
  printf( "EXCEPTION: %d  @0x%04x\n\n", ePC, code );
  exit(0);
}

int main( int argc, const char * argv [] )
{
  if( 2 != argc ) {
    printf("usage: %s input.pdk\n\n", argv[0]);
    return 0;
  }

  struct emuCPU ecpu;
  struct emuCPU* cpu = &ecpu;

  if( emuCPUloadPDK(cpu, argv[1], true) < 0 ) { 
    printf("Error reading input file\n"); 
    return -1; 
  }

  if( !cpu->fnReset || !cpu->fnExecute  ) {
    printf("Missing emulator for this CPU\n"); 
    return -2; 
  }

  //set our own emulation exception handler
  cpu->fnException = emuException;

  //reset CPU
  cpu->fnReset(cpu,true);

  for( uint32_t i=0;i<2000;i++ )
  {
    char disass[64] = "";
    if( cpu->fnDisassemble ) 
      cpu->fnDisassemble(cpu,ePC,disass);

    printf( "C:%10d  A=0x%02x  F=0x%02x  SP=0x%02x  PC=0x%04x:   0x%04x    %s\n", eCycle, eA, eF, eSP, ePC, cpu->eCode[ePC]&0x3FFF, disass );
    
    uint32_t oldPC = ePC;

    int rc = cpu->fnExecute(cpu);
    
    //todo: simulate peripherals
  
    if( 0 != rc )
      break;
      
    //detect endless loop and stop
    if( oldPC == ePC )
      break;
  }

  return 0;
}
