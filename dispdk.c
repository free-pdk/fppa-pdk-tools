#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "emucpu.h"

int main( int argc, const char * argv [] )
{
  int rc;
  if( (argc<2) || (argc>3) ) {
    printf("usage: %s [otpidhex] inputfile\n"
           "example: %s input.pdk\n"
           "example: %s 2AA1 input.bin\n",
           argv[0], argv[0], argv[0]);
    return 0;
  }

  uint16_t otp_id;
  struct emuCPU cpu;

  if( 3 == argc )
  {
    if( 1 != sscanf(argv[1], "%" SCNx16, &otp_id) )
    {
      printf("Error wrong otp id: %s\n",argv[1]);
      return -1; 
    }

    rc=emuCPUloadBIN(&cpu, argv[2], false, otp_id);
  }
  else
  {
    rc=emuCPUloadPDK(&cpu, argv[1], false);
    if( 0 == rc )
    {
      printf("MCU_NAME: %s\n", cpu.hdr.mcu_name);
      printf("OTP_ID: %04X\n", cpu.hdr.otp_id);
    }
    otp_id = cpu.hdr.otp_id;
  }
  
  if( rc < 0 ) {
    if( -4 == rc )
      printf("Error unsupported CPU OTP_ID: %04X\n",otp_id);
    else
    if( -3 == rc )
      printf("Error invalid PDK header\n");
    else
      printf("Error reading input file: %d\n",rc);
    return -1;
  }

  printf("\n");

  char buf[64] = "-----";
  uint32_t p;
  for( p=0; p<cpu.hdr.codesize; p++ )
  {
    if( cpu.fnDisassemble) 
      cpu.fnDisassemble( &cpu, p, buf );
    printf( "0x%04x:   0x%04x    %s\n", p, cpu.eCode[p], buf );
  }

  return 0;
}
