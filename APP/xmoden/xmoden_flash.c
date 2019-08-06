/**
 * @file    uart.c
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module is a layer between the HAL UART functions and my Xmodem protocol.
 *__iar_program_start
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#include "stm8s.h"
#include "stm8s_uart1.h"
#include "xmoden_flash.h"


#define FLASH_TEST (0)
#define FLASH_TIMEOUT   ((uint16_t)0xFFFF)

void Xmoden_FlashInit(void)
{
  FLASH->CR1 = FLASH_CR1_RESET_VALUE;
  FLASH->CR2 = FLASH_CR2_RESET_VALUE;
  FLASH->NCR2 = FLASH_NCR2_RESET_VALUE;
  FLASH->IAPSR &= (uint8_t)(~FLASH_IAPSR_DUL);
  FLASH->IAPSR &= (uint8_t)(~FLASH_IAPSR_PUL);
  (void) FLASH->IAPSR; /* Reading of this register causes the clearing of status flags */
#if FLASH_TEST
  Xmoden_FlashWrite(FLASH_DATA_START_ADDR,0,3);
#endif
}


Xmoden_stFlashStatus Xmoden_FlashErase(uint32_t address)
{
  return FLASH_OK;
}
Xmoden_stFlashStatus Xmoden_FlashRead(uint32_t address, uint8_t *data, uint16_t len)
{
  if(data == 0 || len == 0)
    return FLASH_ERROR;
  for(; len; len--)
    *(data++) = *(PointerAttr uint8_t *) (MemoryAddressCast)(address++);
  return FLASH_OK;
}
Xmoden_stFlashStatus Xmoden_FlashWrite(uint32_t address, uint8_t *data, uint16_t len)
{
  uint16_t i=0;
  uint32_t wData=0;
  uint8_t flagstatus = 0x00;
  uint16_t timeout = FLASH_TIMEOUT;
  
#if FLASH_TEST
uint8_t tbuf[83] = {0};
  len = sizeof(tbuf);
  address = FLASH_DATA_START_ADDR;
  data = tbuf;
  tbuf[0] = *(PointerAttr uint8_t *) (MemoryAddressCast)(address);
  for(i=0; i<len+4; i++)//len must be 4 8 12 16...
  {
    flagstatus = *(PointerAttr uint8_t *) (MemoryAddressCast)(address + i);
    Xmoden_UartTransmitByte(flagstatus);
  }
  if(tbuf[0] == '+')
  {
    tbuf[0] = '-';
    for(i=1; i<len; i++)
      tbuf[i] = i + 0x10;
  }
  else
  {
    tbuf[0] = '+';
    for(i=1; i<len; i++)
      tbuf[i] = i + 0x50;
  }
#else
  if(data == 0 || len == 0)
    return FLASH_ERROR;
#endif
  
  FLASH->PUKR = FLASH_RASS_KEY1;
  FLASH->PUKR = FLASH_RASS_KEY2;
  while((FLASH->IAPSR & FLASH_FLAG_PUL) == RESET);
  
  
  wData = *data;
  for(i=1; i<len; i++)//len must be 4 8 12 16...
  {
    wData <<= 8;
    wData |= *(data + i);
    
    if((i & 3) == 3)
    {
      FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);
      FLASH->CR1 |= (uint8_t)FLASH_PROGRAMTIME_STANDARD;//Set Standard programming time,(erase and write)
  
      FLASH->CR2 |= FLASH_CR2_WPRG;
      FLASH->NCR2 &= (uint8_t)(~FLASH_NCR2_NWPRG);//Set Word Programming
      
      /* Write Word - from lowest address*/
      *((PointerAttr uint8_t*)(MemoryAddressCast)address)       = *((uint8_t*)(&wData));
      *(((PointerAttr uint8_t*)(MemoryAddressCast)address) + 1) = *((uint8_t*)(&wData)+1); 
      *(((PointerAttr uint8_t*)(MemoryAddressCast)address) + 2) = *((uint8_t*)(&wData)+2); 
      *(((PointerAttr uint8_t*)(MemoryAddressCast)address) + 3) = *((uint8_t*)(&wData)+3); 
      address += 4;
      
      while((flagstatus == 0x00) && (timeout != 0x00))
      {
        flagstatus = (uint8_t)(FLASH->IAPSR & (uint8_t)(FLASH_IAPSR_EOP |
                                                        FLASH_IAPSR_WR_PG_DIS));
        if(timeout && --timeout == 0)
          return FLASH_ERROR;
      }
      
      wData = 0;
    }
  }
  
  len %= 4;
  if(len)
  {
    i -= len;
    wData = (uint32_t)*(data + i++) << 24;
    wData |= (uint32_t)*(data + i++) << 16;
    wData |= (uint32_t)*(data + i++) << 8;
  
    FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);
    FLASH->CR1 |= (uint8_t)FLASH_PROGRAMTIME_STANDARD;//Set Standard programming time,(erase and write)

    FLASH->CR2 |= FLASH_CR2_WPRG;
    FLASH->NCR2 &= (uint8_t)(~FLASH_NCR2_NWPRG);//Set Word Programming
    
    /* Write Word - from lowest address*/
    *((PointerAttr uint8_t*)(MemoryAddressCast)address)       = *((uint8_t*)(&wData));
    *(((PointerAttr uint8_t*)(MemoryAddressCast)address) + 1) = *((uint8_t*)(&wData)+1); 
    *(((PointerAttr uint8_t*)(MemoryAddressCast)address) + 2) = *((uint8_t*)(&wData)+2); 
    *(((PointerAttr uint8_t*)(MemoryAddressCast)address) + 3) = *((uint8_t*)(&wData)+3); 
    address += 4;
    
    while((flagstatus == 0x00) && (timeout != 0x00))
    {
      flagstatus = (uint8_t)(FLASH->IAPSR & (uint8_t)(FLASH_IAPSR_EOP |
                                                      FLASH_IAPSR_WR_PG_DIS));
      if(timeout && --timeout == 0)
        return FLASH_ERROR;
    }
  }
  
//  for(i=0; i<len; i++)//len must be 4 8 12 16...
//  {
//    flagstatus = *(PointerAttr uint8_t *) (MemoryAddressCast)(address + i);
//  }
  
  FLASH->IAPSR &= (uint8_t)FLASH_MEMTYPE_PROG;
  return FLASH_OK;
}