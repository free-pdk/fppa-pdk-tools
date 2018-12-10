#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pdkformat.h"

int main( int argc, const char * argv [] )
{
  uint8_t datain[10000];
  uint8_t dataout[10000];

  if( 3 != argc ) {
    printf("usage: %s inputfile outputfile\n\n", argv[0]);
    return 0;
  }

  FILE* fin = fopen( argv[1], "rb");
  if( !fin ) { printf("Could not open %s for reading.\n", argv[1]); return -1; }
  int32_t datainlen = fread( datain, 1, sizeof(datain), fin );
  if( datainlen<=0 ) { printf("Error reading input file\n");return -1; }
  fclose(fin);
  
  int32_t dataoutlen = depdk( datain, datainlen, dataout, sizeof(dataout) );
  if( dataoutlen<=0 ) { printf("Error decrypting input file\n");return -1; }

  int32_t hdrlen = pdkhdrlen(datain,datainlen);

  uint32_t fcsum = *((uint32_t*)&datain[0x0C]);
  *((uint32_t*)&datain[0x0C]) = 0;

  uint32_t csum1 = pdkchecksum( datain, hdrlen, 0 );
  uint32_t csum2 = pdkchecksum( dataout, dataoutlen, 0 );
//  uint32_t csum2 = pdkchecksum( datain+..., ..., csum2 );

  uint32_t csumh = csum1+csum2;
  
  if(csumh!=fcsum)
    printf("WARNING: FILE CHECKSUM INCORRECT\n");

  uint32_t csums = csum2&0xFFFFFF;
  if( 0xFFFF==(csums&0xFFFF) )
    csums--;
  else
  if( 0==csums )
    csums++;
    
  uint32_t fcsiz = *((uint32_t*)&datain[0x20]);
  switch( fcsiz )
  {
    case 1024: csums&=0xFF1FFF; break;
    case 2048: csums&=0xFF3FFF; break;
    case 4096: csums&=0xFF7FFF; break;
  }

  printf("CHECKSUM IDE: %06X\n", csums);

  FILE* fout = fopen( argv[2], "wb");
  if( !fin ) { printf("Could not open %s for writing.\n", argv[2]); return -1; }
  if( dataoutlen != fwrite( dataout, 1, dataoutlen, fout ) ) { printf("Error writing output file\n"); return -1; }
  fclose(fout);

  return 0;
}
