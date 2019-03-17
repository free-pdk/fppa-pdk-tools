#ifndef __INC_CPUVARIANT_H__
#define __INC_CPUVARIANT_H__

#include "emucpu.h"

//void pmx130_init(struct emuCPU *cpu, bool fixupHighCode);

void pmx150_init(struct emuCPU *cpu, bool fixupHighCode);

void pmx154_init(struct emuCPU *cpu, bool fixupHighCode);

void pmx173_init(struct emuCPU *cpu, bool fixupHighCode);

int generic_init(struct emuCPU *cpu);


#endif //__INC_CPUVARIANT_H__
