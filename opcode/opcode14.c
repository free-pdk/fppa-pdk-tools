#include <stdint.h>
#include <stdbool.h>

#include "emucpu.h"

//execute next 14 bit opcode and advanced PC
int opcode14(struct emuCPU *cpu)
{
  int16_t T;                                                                                                                        //temp register
  uint16_t opcode = emuCPUcodeGet( cpu, ePC++ ) & 0x3FFF;                                                                           //fetch next opcode and advance PC
  eCycle++;                                                                                                                         //increment current cycle counter

  //14 bit opcodes 0x0000 - 0x00BF
  if( opcode<=0x00BF )
  {
    switch( opcode )
    {
      case 0x0000: break;                                                                                                           //NOP

      case 0x0006: eA=emuCPUcodeGet(cpu,emuCPUmemGet(cpu,eSP)|(((uint16_t)emuCPUmemGet(cpu,eSP+1))<<8))&0xFF; eCycle++; break;      //LDSPTL //TODO: verify, Z?
      case 0x0007: eA=emuCPUcodeGet(cpu,emuCPUmemGet(cpu,eSP)|(((uint16_t)emuCPUmemGet(cpu,eSP+1))<<8))>>8; eCycle++; break;        //LDSPTH //TODO: verify, Z?

      case 0x0060: T=(eF>>1)&1;eF=emuCPUaddSolveFlagsVACZ(eA,0,T); eA=(eA+T)&0xFF; break;                                           //ADDC A
      case 0x0061: T=(eF>>1)&1;eF=emuCPUsubSolveFlagsVACZ(eA,0,T); eA=(eA-T)&0xFF; break;                                           //SUBC A
      case 0x0062: eF=emuCPUaddSolveFlagsVACZ(eA, 1, 0); eA=(eA+1)&0xFF; if(!eA){ePC++; eCycle++;} break;                           //IZSN A
      case 0x0063: eF=emuCPUsubSolveFlagsVACZ(eA, 1, 0); eA=(eA-1)&0xFF; if(!eA){ePC++; eCycle++;} break;                           //DZSN A

      case 0x0067: ePC=(ePC-1)+eA; break;                                                                                           //PCADD A
      case 0x0068: eA=(~eA)&0xFF; eF=(eF&0xE)|(!eA); break;                                                                         //NOT A
      case 0x0069: eA=(-((int8_t)eA))&0xFF; eF=(eF&0xE)|(!eA); break;                                                               //NEG A
      case 0x006A: eF=(eF&0xD)|((eA<<1)&2); eA>>=1; break;                                                                          //SR A
      case 0x006B: eF=(eF&0xD)|((eA>>6)&2); eA=(eA<<1)&0xFF; break;                                                                 //SL A
      case 0x006C: eA|=(eF&2)<<7; eF=(eF&0xD)|((eA<<1)&2); eA>>=1; break;                                                           //SRC A
      case 0x006D: T=(eF>>1)&1; eF=(eF&0xD)|((eA>>6)&2); eA=((eA<<1)&0xFF)|T; break;                                                //SLC A
      case 0x006E: eA=((eA<<4)|(eA>>4))&0xFF; break;                                                                                //SWAP A

      case 0x0070: /*TODO*/ break; //WDRESET

      case 0x0072: emuCPUstackPush(cpu,eA); emuCPUstackPush(cpu,eF); break;                                                         //PUSHAF
      case 0x0073: eF=emuCPUstackPop(cpu); eA=emuCPUstackPop(cpu); break;                                                           //POPAF

      case 0x0075: return 1;                                                                                                        //RESET
      case 0x0076: ePC--; return 2;                                                                                                 //STOPSYS
      case 0x0077: ePC--; return 3;                                                                                                 //STOPEXE
      case 0x0078: eGINTEnabled=true;  break;                                                                                       //ENGINT
      case 0x0079: eGINTEnabled=false;  break;                                                                                      //DISGINT
      case 0x007A: ePC=emuCPUstackPopW(cpu); break;                                                                                 //RET
      case 0x007B: ePC=emuCPUstackPopW(cpu); eGINTEnabled=true; break;                                                              //RETI
      case 0x007C: /*TODO*/ break; //MUL

      default:
        emuCPUexception( cpu, EXCEPTION_ILLEGAL_OPCODE );
        return -1;
    }
  }
  else
  //8 bit opcodes 0x00C0 - 0x01FF
  if( (opcode>=0x00C0) && (opcode<=0x01FF) )
  {
    uint8_t addr = opcode&0x3F;
    switch( opcode&0x3FC0 )
    {
      case 0x00C0: emuCPUioPut(cpu,addr,emuCPUioGet(cpu,addr)^eA);break;                                                            //XOR IO, A
      case 0x0180: emuCPUioPut(cpu,addr,eA);break;                                                                                  //MOV IO, A
      case 0x01C0: eA=emuCPUioGet(cpu,addr);eF=(eF&0xE)|(!eA);break;                                                                //MOV A, IO
      default:
        emuCPUexception(cpu,EXCEPTION_ILLEGAL_OPCODE);
        return -1;
    }
  }
  else
  //7 bit opcodes 0x03..
  if( 0x0300 == (opcode&0x3F00) )
  {
    uint8_t addr = opcode&0x7E;
    switch( opcode & 0x3F81 )
    {
      case 0x0300: emuCPUmemPut(cpu, addr, eT16&0xFF); emuCPUmemPut(cpu, addr, eT16>>8); break;                                     //STT16 M
      case 0x0301: eT16 = emuCPUmemGet(cpu, addr) | (((uint16_t)emuCPUmemGet(cpu, addr+1))<<8); break;                              //LDT16 M
      case 0x0380: T=emuCPUmemGet(cpu, addr); eCycle++; emuCPUmemPut(cpu, T,eA); break;                                             //IDXM M,A
      case 0x0381: T=emuCPUmemGet(cpu, addr); eCycle++; eA=emuCPUmemGet(cpu, T); break;                                             //IDXM A,M
    }
  }
  else
  //7 bit opcodes 0x0600 - 0x17FF
  if( (opcode>=0x0600) && (opcode<=0x17FF) )
  {
    int8_t addr = opcode&0x7F;
    int16_t M = emuCPUmemGet(cpu, addr);
    switch( opcode & 0x3F80 )
    {
      case 0x0600: eF=emuCPUsubSolveFlagsVACZ(eA,M,0); break;                                                                       //COMP A, M
      case 0x0680: eF=emuCPUsubSolveFlagsVACZ(M,eA,0); break;                                                                       //COMP M, A
      case 0x0700: eF=emuCPUaddSolveFlagsVACZ(eA,(-M)&0xFF,0); eA=(eA+((-M)&0xFF))&0xFF; break;                                     //NADD A, M //TODO: verify
      case 0x0780: eF=emuCPUaddSolveFlagsVACZ(M,(-eA)&0xFF,0); M=(M+((-eA)&0xFF))&0xFF; break;                                      //NADD M, A //TODO: verify
      case 0x0800: eF=emuCPUaddSolveFlagsVACZ(M,eA,0); emuCPUmemPut(cpu,addr,M+eA); break;                                          //ADD M, A
      case 0x0880: eF=emuCPUsubSolveFlagsVACZ(M,eA,0); emuCPUmemPut(cpu,addr,M-eA); break;                                          //SUB M, A
      case 0x0900: T=(eF>>1)&1;eF=emuCPUaddSolveFlagsVACZ(M,eA,T); emuCPUmemPut(cpu,addr,M+eA+T); break;                            //ADDC M, A
      case 0x0980: T=(eF>>1)&1;eF=emuCPUsubSolveFlagsVACZ(M,eA,T); emuCPUmemPut(cpu,addr,M-eA-T); break;                            //SUBC M, A
      case 0x0A00: M&=eA;emuCPUmemPut(cpu,addr,M);eF=(eF&0xE)|(!M); break;                                                          //AND M, A
      case 0x0A80: M|=eA;emuCPUmemPut(cpu,addr,M);eF=(eF&0xE)|(!M); break;                                                          //OR M, A
      case 0x0B00: M^=eA;emuCPUmemPut(cpu,addr,M);eF=(eF&0xE)|(!M); break;                                                          //XOR M, A
      case 0x0B80: emuCPUmemPut(cpu,addr,eA); break;                                                                                //MOV M, A
      case 0x0C00: eF=emuCPUaddSolveFlagsVACZ(eA,M,0);eA=(eA+M)&0xFF; break;                                                        //ADD A, M
      case 0x0C80: eF=emuCPUsubSolveFlagsVACZ(eA,M,0);eA=(eA-M)&0xFF; break;                                                        //SUB A, M
      case 0x0D00: T=(eF>>1)&1;eF=emuCPUaddSolveFlagsVACZ(eA,M,T);eA=(eA+M+T)&0xFF; break;                                          //ADDC A, M
      case 0x0D80: T=(eF>>1)&1;eF=emuCPUsubSolveFlagsVACZ(eA,M,T);eA=(eA-M-T)&0xFF; break;                                          //SUBC A, M
      case 0x0E00: eA=eA&M;eF=(eF&0xE)|(!eA); break;                                                                                //AND A, M
      case 0x0E80: eA=eA|M;eF=(eF&0xE)|(!eA); break;                                                                                //OR A, M
      case 0x0F00: eA=eA^M;eF=(eF&0xE)|(!eA); break;                                                                                //XOR A, M
      case 0x0F80: eA=M;eF=(eF&0xE)|(!eA); break;                                                                                   //MOV A, M
      case 0x1000: T=(eF>>1)&1;eF=emuCPUaddSolveFlagsVACZ(M,0,T); emuCPUmemPut(cpu,addr,M+T); break;                                //ADDC M
      case 0x1080: T=(eF>>1)&1;eF=emuCPUsubSolveFlagsVACZ(M,0,T); emuCPUmemPut(cpu,addr,M-T); break;                                //SUBC M
      case 0x1100: eF=emuCPUaddSolveFlagsVACZ(M,1,0);emuCPUmemPut(cpu,addr,M+1);if(!(eF&1)){ePC++;eCycle++;}break;                  //IZSN M
      case 0x1180: eF=emuCPUsubSolveFlagsVACZ(M,1,0);emuCPUmemPut(cpu,addr,M-1);if(!(eF&1)){ePC++;eCycle++;}break;                  //DZSN M
      case 0x1200: eF=emuCPUaddSolveFlagsVACZ(M,1,0);emuCPUmemPut(cpu,addr,M+1);break;                                              //INC M
      case 0x1280: eF=emuCPUsubSolveFlagsVACZ(M,1,0);emuCPUmemPut(cpu,addr,M-1);break;                                              //DEC M
      case 0x1300: emuCPUmemPut(cpu,addr,0); break;                                                                                 //CLEAR M
      case 0x1380: emuCPUmemPut(cpu,addr,eA); eA=M; break;                                                                          //XCH A,M
      case 0x1400: M=(~M)&0xFF; eF=(eF&0xE)|(!M); emuCPUmemPut(cpu,addr,M); break;                                                  //NOT M
      case 0x1480: M=(-((int8_t)M))&0xFF; eF=(eF&0xE)|(!M); emuCPUmemPut(cpu,addr,M); break;                                        //NEG M
      case 0x1500: eF=(eF&0xD)|((M<<1)&2); emuCPUmemPut(cpu,addr,M>>1); break;                                                      //SR M
      case 0x1580: eF=(eF&0xD)|((M>>6)&2); emuCPUmemPut(cpu,addr,M<<1); break;                                                      //SL M
      case 0x1600: M|=(eF&2)<<7; eF=(eF&0xD)|((M<<1)&2); emuCPUmemPut(cpu,addr,M>>1); break;                                        //SRC M
      case 0x1680: T=(eF>>1)&1; eF=(eF&0xD)|((M>>6)&2); emuCPUmemPut(cpu,addr,(M<<1)|T); break;                                     //SLC M
      case 0x1700: eF=emuCPUsubSolveFlagsVACZ(eA, M, 0); if( !((eA-M)&0xFF) ){ ePC++; eCycle++;} break;                             //CEQSN A,M
      case 0x1780: eF=emuCPUsubSolveFlagsVACZ(eA, M, 0); if( (eA-M)&0xFF ){ ePC++; eCycle++;} break;                                //CNEQSN A,M
    }
  }
  else
  //6 bit opcodes 0x02.. , 0x2800 - 0x2FFF
  if( (0x0200 == (opcode&0x3F00)) || (0x2800 == (opcode&0x3800)) )
  {
    uint8_t k = opcode&0xFF;
    switch( opcode & 0x3F00 )
    {
      case 0x0200: eA=k; ePC=emuCPUstackPopW(cpu); break;                                                                           //RET k
      case 0x2800: eF=emuCPUaddSolveFlagsVACZ(eA, k, 0); eA=(eA+k)&0xFF; break;                                                     //ADD A,k
      case 0x2900: eF=emuCPUsubSolveFlagsVACZ(eA, k, 0); eA=(eA-k)&0xFF; break;                                                     //SUB A,k
      case 0x2A00: eF=emuCPUsubSolveFlagsVACZ(eA, k, 0); if( !((eA-k)&0xFF) ){ ePC++; eCycle++;} break;                             //CEQSN A,k
      case 0x2B00: eF=emuCPUsubSolveFlagsVACZ(eA, k, 0); if( (eA-k)&0xFF ){ ePC++; eCycle++;} break;                                //CNEQSN A,k
      case 0x2C00: eA &= k; eF=(eF&0xE)|(!eA); break;                                                                               //AND A,k
      case 0x2D00: eA |= k; eF=(eF&0xE)|(!eA); break;                                                                               //OR A,k
      case 0x2E00: eA ^= k; eF=(eF&0xE)|(!eA); break;                                                                               //XOR A,k
      case 0x2F00: eA  = k; break;                                                                                                  //MOV A,k
    }
  }                
  else
  //5 bit opcodes 0x0400 - 0x0500, 0x1800 - 0x27FF
  if( (0x0400 == (opcode&0x3E00)) || ((opcode>=0x1800) && (opcode<=0x27FF)) )
  {
    uint8_t bit = 1<<((opcode>>6)&7);
    uint8_t addr = opcode&0x3F;
    switch( opcode & 0x3E00 )
    {
      case 0x0400: T=emuCPUioGet(cpu,addr);emuCPUioPut(cpu,addr,eF&2?T|bit:T&~bit);eF=(eF&0xD)|((T&bit)?2:0); break;                //SWAPC IO.n
      case 0x1800: if( !(emuCPUioGet(cpu,addr)&bit) ) { ePC++; eCycle++; } break;                                                   //T0SN IO.n
      case 0x1A00: if( emuCPUioGet(cpu,addr)&bit ) { ePC++; eCycle++; } break;                                                      //T1SN IO.n
      case 0x1C00: emuCPUioPut(cpu,addr,emuCPUioGet(cpu,addr)&~bit); break;                                                         //SET0 IO.n
      case 0x1E00: emuCPUioPut(cpu,addr,emuCPUioGet(cpu,addr)|bit); break;                                                          //SET1 IO.n
      case 0x2000: if( !(emuCPUmemGet(cpu,addr)&bit) ) { ePC++; eCycle++; } break;                                                  //T0SN M.n
      case 0x2200: if( emuCPUmemGet(cpu,addr)&bit ) { ePC++; eCycle++; } break;                                                     //T1SN M.n
      case 0x2400: emuCPUmemPut(cpu,addr,emuCPUmemGet(cpu,addr)&~bit);break;                                                        //SET0 M.n
      case 0x2600: emuCPUmemPut(cpu,addr,emuCPUmemGet(cpu,addr)|bit);break;                                                         //SET1 M.n
    }
  }
  else
  //3 bit opcodes 0x3000 - 0x3FFF
  if( (0x3000 == (opcode&0x3000)) )
  {
    if( opcode & 0x0800 ) //CALL needs to put current PC on stack                                                                   //CALL p
      emuCPUstackPushW(cpu,ePC);
    eCycle++;
    ePC = opcode & 0x07FF;                                                                                                          //GOTO p
  }
  else
  {
    emuCPUexception( cpu, EXCEPTION_ILLEGAL_OPCODE );
    return -1;
  }

  return 0;
}
