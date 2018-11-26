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

  FILE* fout = fopen( argv[2], "wb");
  if( !fin ) { printf("Could not open %s for writing.\n", argv[2]); return -1; }
  if( dataoutlen != fwrite( dataout, 1, dataoutlen, fout ) ) { printf("Error writing output file\n"); return -1; }
  fclose(fout);

  return 0;
}
