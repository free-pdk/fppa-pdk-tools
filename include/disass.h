#ifndef __INC_DISASS_H__
#define __INC_DISASS_H__

#include <stdint.h>
#include "emucpu.h"

char* disass13(struct emuCPU *cpu, uint16_t position, char *buffer);

char* disass14(struct emuCPU *cpu, uint16_t position, char *buffer);

//char* disass16(struct emuCPU *cpu, uint16_t position, char *buffer);

#endif //__INC_DISASS_H__
