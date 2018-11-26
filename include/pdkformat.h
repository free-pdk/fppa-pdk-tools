#ifndef __INC_PDKFORMAT_H__
#define __INC_PDKFORMAT_H__

#include <stdint.h>

#pragma pack(push,1)
struct T_PDK_HEADER {
  uint32_t marker;
  uint32_t unk_0x04_1;
  uint32_t version;
  uint32_t checksum;

  uint32_t unk_0x10_0;
  uint32_t unk_0x14_3;
  uint32_t unk_0x18_0x3F30;
  uint32_t unk_0x1C_0;

  uint32_t codesize;
  uint16_t usable_codesize;
  uint16_t extra_length1;
  uint16_t extra_length2;
  uint16_t unk_0x2A_0;
  uint16_t extra_length3;
  uint16_t unk_0x2E_0;

  uint8_t  unk_0x30_0[0xC];

  uint16_t otp_id;
  uint8_t  otp_way;
  uint8_t  boot_type;

  uint8_t  unk_0x40_0[0x1A];

  uint16_t remain_code_free;

  uint8_t  unk_0x5C_0[0x24];

  char     mcu_name[16];
  
  uint8_t  unk_0x90_x[16];

  char     compiler[16];

  uint8_t  unk_0xB0_0[32];

  uint16_t extra_length_4;

  uint8_t  unk_0xD2_0[14];

  uint8_t  key[32];
  
  uint8_t  extradata[256];
};
#pragma pack(pop)

int32_t pdkhdrlen(uint8_t* datain, uint32_t datainlen);
int32_t depdk(uint8_t* datain, uint32_t datainlen, uint8_t* dataout, uint32_t dataoutlen);
int32_t enpdk(uint8_t* hdrin, uint32_t hdrinlen, uint8_t* datain, uint32_t datainlen, uint8_t* dataout, uint32_t dataoutlen);

#endif //__INC_PDKFORMAT_H__
