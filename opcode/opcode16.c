#include <stdint.h>
#include <stdbool.h>

#include "emucpu.h"

//execute next 16 bit opcode and advanced PC
int opcode16(struct emuCPU *cpu)
{
  emuCPUexception(cpu,EXCEPTION_ILLEGAL_OPCODE);
  return -1;
}
