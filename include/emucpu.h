#ifndef __INC_EMUCPU_H__
#define __INC_EMUCPU_H__

#include <stdint.h>
#include <stdbool.h>

#include "pdkformat.h"

#define IO_MAX    64
#define MEM_MAX   512
#define CODEW_MAX 8192

//flags: (1 1 1 1 V A C Z)

#define eA                 cpu->eeA
#define ePC                cpu->eePC
#define eF                 (*((volatile uint8_t*)&cpu->eIO[0x00]))
#define eSP                (*((volatile uint8_t*)&cpu->eIO[0x02]))
#define eGINTEnabled       cpu->eeGINTEnabled
#define eInterruptActive   cpu->eeInterruptActive
#define eT16               cpu->eeT16
#define eCycle             cpu->eeCurrentCycle

struct emuCPU;

// reset cpu
typedef void (*TemuCPUreset)(struct emuCPU *cpu, bool clearRAM);

// execute next opcode: returns number of ticks the operation should take
typedef int (*TemuCPUexecute)(struct emuCPU *cpu); 

// emulate peripherals
typedef void (*TemuCPUperipherals)(struct emuCPU *cpu); 

// decodes opcode at position, and fills the buffer with the assembler code, returns string
typedef char* (*TemuCPUdisassemble)(struct emuCPU *cpu, uint16_t position, char *buffer);

// callback: some exceptional situation occurred. See CPU_EXCEPTION enum, below
typedef void (*TemuCPUexception)(struct emuCPU *cpu, int code);

// callback: an IO port is about to be read. Default is to return the value in IO register. Ports may act differently.
typedef uint8_t (*TemuCPUioRead)(struct emuCPU *cpu, uint8_t port);

// callback: an IO port has changed. Default is to do nothing.
typedef void (*TemuCPUioWrite)(struct emuCPU *cpu, uint8_t port);

// decodes IO name, and fills the buffer with the name / bitpos / ..., returns string
typedef char* (*TemuCPUioName)(struct emuCPU *cpu, uint8_t io, uint8_t bit, char *buffer);


struct emuCPU
{
  struct T_PDK_HEADER hdr;                //pdk header
  uint32_t hdrlen;
  
  uint8_t  eIO[IO_MAX];                   //io (special register)
  uint32_t maxIO;
  uint8_t  eMem[MEM_MAX];                 //memory (RAM)
  uint32_t maxMem;
  uint16_t eCode[CODEW_MAX];              //instruction words (ROM)
  uint32_t maxCode;
  
  uint32_t eePC;
  int16_t  eeA;

  //eeF  mapped with define to IO[0x00] (flags)
  //eeSP mapped with define to IO[0x02] (stack pointer)

  uint16_t eeT16;                         //timer T16 value

  bool     eeGINTEnabled;                  //gloabl interrupt enabled
  bool     eeInterruptActive;              //internal status that cpu started interrupt

  uint32_t eeCurrentCycle;                 //keeps track of current CPU cycle (can be used to synchronize to real time)

  TemuCPUreset       fnReset;
  TemuCPUexecute     fnExecute;
  TemuCPUperipherals fnPeripherals;
  TemuCPUdisassemble fnDisassemble;
  TemuCPUexception   fnException;
  TemuCPUioRead      fnIORead;
  TemuCPUioWrite     fnIOWrite;
  TemuCPUioName      fnIOName;
};

enum CPU_EXCEPTION
{
  EXCEPTION_MEMORY,            // illegal memory access
  EXCEPTION_IO,                // illegal IO access
  EXCEPTION_CODE,              // illegal code access
  EXCEPTION_IMEMORY,           // indirect pointer outside of chip memory
  EXCEPTION_ILLEGAL_OPCODE     // for the single 'reserved' opcode in the architecture
};

//init cpu based on type set in cpu structure. Returns negative for errors.
int emuCPUinit(struct emuCPU *cpu, uint8_t* hdr, uint32_t hdrlen);

//load a PDK file. Returns negative for errors.
int emuCPUloadPDK(struct emuCPU *cpu, const char *filename);

//helper functions for all variants
void     emuCPUexception(struct emuCPU *cpu, int code);
uint8_t  emuCPUioGet(struct emuCPU *cpu, uint8_t addr) __attribute__((always_inline));
void     emuCPUioPut(struct emuCPU *cpu, uint8_t addr, uint8_t dat) __attribute__((always_inline));
char*    emuCPUdecodeIO(struct emuCPU *cpu, uint8_t addr, uint8_t bit, char* buffer);
uint8_t  emuCPUmemGet(struct emuCPU *cpu, uint8_t addr) __attribute__((always_inline));
void     emuCPUmemPut(struct emuCPU *cpu, uint8_t addr, uint8_t dat) __attribute__((always_inline));
uint16_t emuCPUcodeGet(struct emuCPU *cpu, uint16_t addr) __attribute__((always_inline));
void     emuCPUstackPush(struct emuCPU *cpu, uint8_t dat) __attribute__((always_inline));
uint8_t  emuCPUstackPop(struct emuCPU *cpu) __attribute__((always_inline));
void     emuCPUstackPushW(struct emuCPU *cpu, uint16_t dat) __attribute__((always_inline));
uint16_t emuCPUstackPopW(struct emuCPU *cpu) __attribute__((always_inline));
uint8_t  emuCPUaddSolveFlagsVACZ(int16_t value1, int16_t value2, int16_t c) __attribute__((always_inline));
uint8_t  emuCPUsubSolveFlagsVACZ(int16_t value1, int16_t value2, int16_t c) __attribute__((always_inline));

#endif // __INC_EMUCPU_H__

