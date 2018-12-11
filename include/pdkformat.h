#ifndef __INC_PDKFORMAT_H__
#define __INC_PDKFORMAT_H__

#include <stdint.h>

#pragma pack(push,1)
struct T_PDK_HEADER {
  uint32_t marker;              //0x00
  uint32_t unk_0x04_1;          //0x04
  uint32_t version;             //0x08
  uint32_t checksum;            //0x0C

  uint32_t tuning_freq_half;    //0x10
  uint16_t unk_0x14_3;          //0x14
  uint16_t tuning_voltage_mv;   //0x16
  uint32_t unk_0x18_0x3F30;     //0x18
  uint32_t unk_0x1C_0;          //0x1C

  uint32_t codesize;            //0x20
  uint16_t usable_codesize;     //0x24
  uint16_t extra_length1;       //0x26
  uint16_t extra_length2;       //0x28
  uint16_t unk_0x2A_0;          //0x2A
  uint16_t extra_length3;       //0x2C
  uint16_t unk_0x2E_0;          //0x2E

  uint8_t  unk_0x30_0[0xC];     //0x30

  uint16_t otp_id;              //0x3C
  uint8_t  otp_way;             //0x3E
  uint8_t  boot_type;           //0x3F

  uint8_t  unk_0x40_0[8];       //0x40

  uint8_t  extra_length_pinmap; //0x49

  uint8_t  unk_0x4A_0[16];      //0x4A

  uint16_t remain_code_free;    //0x5A

  uint16_t unk_0x5C;            //0x5C
  uint16_t unk_0x5D_maybe_lvd;  //0x5E
  
  uint8_t  unk_0x60_0[0x20];    //0x60

  char     mcu_name[16];        //0x80
  
  uint8_t  unk_0x90_x[16];      //0x90

  char     compiler[16];        //0xA0

  uint8_t  unk_0xB0_0[32];      //0xB0

  uint16_t extra_length_4;      //0xD0

  uint8_t  unk_0xD2_0[14];      //0xD2

  uint8_t  key[32];             //0xE0
  
  uint8_t  extradata[256];      //0x100
};
#pragma pack(pop)

int32_t  pdkhdrlen(uint8_t* datain, uint32_t datainlen);
uint32_t pdkchecksum( uint8_t* datain, uint32_t datainlen, uint32_t initval);
int32_t  depdk(uint8_t* datain, uint32_t datainlen, uint8_t* dataout, uint32_t dataoutlen);
int32_t  enpdk(uint8_t* hdrin, uint32_t hdrinlen, uint8_t* datain, uint32_t datainlen, uint8_t* dataout, uint32_t dataoutlen);

#endif //__INC_PDKFORMAT_H__
