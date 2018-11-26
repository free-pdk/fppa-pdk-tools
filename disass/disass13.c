#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "emucpu.h"

char* disass13(struct emuCPU *cpu, uint16_t position, char *buffer)
{
  strcpy( buffer, "?????" );
  return buffer;
}
