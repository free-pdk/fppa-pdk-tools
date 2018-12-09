#include <stdint.h>
#include <stdbool.h>

#include "emucpu.h"

//execute next 15 bit opcode and advanced PC
int opcode15(struct emuCPU *cpu)
{
  emuCPUexception(cpu,EXCEPTION_ILLEGAL_OPCODE);
  return -1;
}
